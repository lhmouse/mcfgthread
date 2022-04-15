// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_STARTUP_C_  1
#include "thread.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "win32.h"

extern char __my_image_base_from_gnu_ld __asm__("__image_base__");

// Define static data, which are declared in 'fwd.h'.
const HANDLE _MCF_crt_module = &__my_image_base_from_gnu_ld;
HANDLE __MCF_crt_heap;
uint32_t __MCF_win32_tls_index;
double __MCF_perf_frequency_reciprocal;
_MCF_thread __MCF_main_thread;

_MCF_mutex __MCF_cxa_atexit_mutex;
__MCF_dtor_queue __MCF_cxa_atexit_queue;
_MCF_mutex __MCF_cxa_at_quick_exit_mutex;
__MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;

// Define startup and cleanup routines.
void
__MCF_initialize(void)
  {
    // Create the CRT heap for memory allocation.
    __MCFGTHREAD_CHECK(__MCF_crt_heap == NULL);
    __MCF_crt_heap = GetProcessHeap();
    __MCFGTHREAD_CHECK(__MCF_crt_heap);

    // Allocate a TLS slot for this library.
    __MCF_win32_tls_index = TlsAlloc();
    __MCFGTHREAD_CHECK(__MCF_win32_tls_index != TLS_OUT_OF_INDEXES);

    // Get the performance counter resolution.
    LARGE_INTEGER freq;
    __MCFGTHREAD_CHECK(QueryPerformanceFrequency(&freq));
    __MCF_perf_frequency_reciprocal = 1000 / (double) freq.QuadPart;

    // Attach the main thread.
    __MCF_main_thread.__tid = GetCurrentThreadId();
    __MCF_main_thread.__handle = OpenThread(THREAD_ALL_ACCESS, FALSE, __MCF_main_thread.__tid);
    __MCFGTHREAD_CHECK(__MCF_main_thread.__handle);
    __MCF_ATOMIC_STORE_REL(__MCF_main_thread.__nref, 1);
    __MCFGTHREAD_CHECK(TlsSetValue(__MCF_win32_tls_index, &__MCF_main_thread));
  }

void
__MCF_finalize_on_thread_exit(void)
  {
    _MCF_thread* self = _MCF_thread_self();
    if(!self)
      return;

    // Call destructors for thread-local objects.
    __MCF_dtor_queue_finalize(&(self->__atexit_queue), NULL, NULL);

    // Call destructors and callbacks registered with `__cxa_thread_atexit()`.
    __MCF_tls_table_finalize(&(self->__tls_table));

    // Detach the current thread.
    (void) TlsSetValue(__MCF_win32_tls_index, NULL);
    _MCF_thread_drop_ref_nonnull(self);
  }

void
__MCF_finalize_on_process_exit(void)
  {
    // Call destructors for thread-local objects before static ones, in
    // accordance with the C++ standard. See [basic.start.term]/2.
    __MCF_finalize_on_thread_exit();

    // Call destructors and callbacks registered with `__cxa_atexit()` and
    // `atexit()`.
    __MCF_dtor_queue_finalize(&__MCF_cxa_atexit_queue, &__MCF_cxa_atexit_mutex, NULL);
  }

static void __stdcall
TlsCRTStartup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    __MCFGTHREAD_ASSERT(instance == &__my_image_base_from_gnu_ld);
    (void) reserved;

    // Perform global initialization and per-thread cleanup, as needed.
    // Note, upon `DLL_PROCESS_DETACH` we don't perform any cleanup, because
    // other DLLs might have been unloaded and we would be referencing unmapped
    // memory. User code should call `__cxa_finalize(NULL)` before exiting from
    // a process.
    if(reason == DLL_PROCESS_ATTACH)
      __MCF_initialize();

    if(reason == DLL_THREAD_DETACH)
      __MCF_finalize_on_thread_exit();
  }

#ifdef DLL_EXPORT

// When building the shared library, invoke the common routine from the DLL
// entry point callback.
int __stdcall
DllMainCRTStartup(HANDLE instance, DWORD reason, LPVOID reserved);

int __stdcall
DllMainCRTStartup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    // Prevent this DLL from being unloaded.
    HMODULE locked;
    __MCFGTHREAD_CHECK(GetModuleHandleExW(5, instance, &locked));
    __MCFGTHREAD_CHECK(locked == instance);

    // Call the common routine. This will not fail.
    TlsCRTStartup(instance, reason, reserved);
    return TRUE;
  }

#else  // DLL_EXPORT

// When building the static library, invoke the common routine from a TLS
// callback. This requires the main executable be linked with 'tlssup.o'.
// `__xl_d` is being used by mingw-w64, so we use `__xl_e` here.
extern const PIMAGE_TLS_CALLBACK __MCF_xl_e;

const PIMAGE_TLS_CALLBACK __MCF_xl_e
  __attribute__((__section__(".CRT$XLE"), __used__)) = TlsCRTStartup;

#endif  // DLL_EXPORT
