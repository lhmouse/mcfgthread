/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_DYNCONST
#include "thread.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "win32.h"

__MCF_HANDLE _MCF_crt_module;
__MCF_HANDLE __MCF_crt_heap;
uint32_t __MCF_win32_tls_index;
double __MCF_perf_frequency_reciprocal;
_MCF_thread __MCF_main_thread;

_MCF_mutex __MCF_cxa_atexit_mutex;
__MCF_dtor_queue __MCF_cxa_atexit_queue;
_MCF_mutex __MCF_cxa_at_quick_exit_mutex;
__MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;
uint8_t __MCF_mutex_spin_field[];

uint32_t
_MCF_get_win32_error(void)
  {
    return GetLastError();
  }

static inline
void
do_startup_thread_finalize(void)
  {
    _MCF_thread* self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return;

    /* Per-thread atexit callbacks may use TLS, so call them before
     * destructors of thread-local objects.  */
    __MCF_dtor_queue_finalize(&(self->__atexit_queue), NULL, NULL);
    __MCF_tls_table_finalize(&(self->__tls_table));
  }

static inline
void
do_startup_thread_detach_self(void)
  {
    _MCF_thread* self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return;

    TlsSetValue(__MCF_win32_tls_index, NULL);
    _MCF_thread_drop_ref_nonnull(self);
  }

void
__MCF_finalize_on_exit(void)
  {
    /* Call destructors for thread-local objects before static ones, in
     * accordance with the C++ standard. See [basic.start.term]/2.  */
    do_startup_thread_finalize();
    __MCF_dtor_queue_finalize(&__MCF_cxa_atexit_queue, &__MCF_cxa_atexit_mutex, NULL);
    do_startup_thread_detach_self();
  }

/* Define the common routine for both static and shared libraries.  */
static
void __stdcall
do_image_tls_callback(PVOID instance, DWORD reason, LPVOID reserved)
  {
    /* Perform global initialization and per-thread cleanup, as needed.
     * Note, upon `DLL_PROCESS_DETACH` we don't perform any cleanup, because
     * other DLLs might have been unloaded and we would be referencing unmapped
     * memory. User code should call `__cxa_finalize(NULL)` before exiting from
     * a process.  */
    if(reason == DLL_PROCESS_ATTACH) {
      if(_MCF_crt_module)
        return;

      /* Set up the base address in memory.  */
      _MCF_crt_module = instance;
      __MCFGTHREAD_CHECK(_MCF_crt_module);

      /* Create the CRT heap for memory allocation.  */
      __MCF_crt_heap = GetProcessHeap();
      __MCFGTHREAD_CHECK(__MCF_crt_heap);

      /* Allocate a TLS slot for this library.  */
      __MCF_win32_tls_index = TlsAlloc();
      __MCFGTHREAD_CHECK(__MCF_win32_tls_index != TLS_OUT_OF_INDEXES);

      /* Get the performance counter resolution.  */
      LARGE_INTEGER freq;
      __MCFGTHREAD_CHECK(QueryPerformanceFrequency(&freq));
      __MCF_perf_frequency_reciprocal = 1000 / (double) freq.QuadPart;

      /* Attach the main thread.  */
      DWORD main_tid = GetCurrentThreadId();
      __MCF_main_thread.__tid = main_tid;
      __MCF_main_thread.__handle = OpenThread(THREAD_ALL_ACCESS, FALSE, main_tid);
      __MCFGTHREAD_CHECK(__MCF_main_thread.__handle);
      __MCF_ATOMIC_STORE_REL(__MCF_main_thread.__nref, 1);
      __MCFGTHREAD_CHECK(TlsSetValue(__MCF_win32_tls_index, &__MCF_main_thread));
    }
    else if(reason == DLL_THREAD_DETACH) {
      do_startup_thread_finalize();
      do_startup_thread_detach_self();
    }

    UNREFERENCED_PARAMETER(reserved);
  }

#ifdef DLL_EXPORT

/* When building the shared library, invoke the common routine from the DLL
 * entry point callback. The decorated name is fabricated such that it remains
 * the same on both x86 and x86-64.  */
int __stdcall
__MCF_dll_startup(PVOID instance, DWORD reason, PVOID reserved)
  __asm__("__MCF_dll_startup@@Z");

int __stdcall
__MCF_dll_startup(PVOID instance, DWORD reason, PVOID reserved)
  {
    if(reason == DLL_PROCESS_ATTACH) {
      /* Prevent this DLL from being unloaded.  */
      HMODULE locked;
      __MCFGTHREAD_CHECK(GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
            (void*)instance, &locked));
    }

    /* Call the common routine. This will not fail.  */
    do_image_tls_callback(instance, reason, reserved);
    return TRUE;
  }

#else  /* DLL_EXPORT

 * When building the static library, invoke the common routine from a TLS
 * callback. This requires the main executable be linked with 'tlssup.o'.
 * `__xl_d` is being used by mingw-w64, so we use `__xl_e` here.  */
extern const PIMAGE_TLS_CALLBACK __MCF_xl_e;

const PIMAGE_TLS_CALLBACK __MCF_xl_e
  __attribute__((__section__(".CRT$XLE"), __used__)) = do_image_tls_callback;

#endif  /* DLL_EXPORT  */
