// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_STARTUP_C_  1
#include "thread.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "win32.h"

extern char __my_image_base_from_gnu_ld
  __asm__("__image_base__");

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

// Define a function that is shared between dynamic and static libraries.
static void
do_startup_common(DWORD reason)
  {
    if(reason == DLL_PROCESS_ATTACH) {
      // Create the CRT heap for memory allocation.
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
    else if(reason == DLL_THREAD_DETACH) {
      // Perform per-thread cleanup.
      // This is not done at process detach as it is too late. A thread that
      // wishes to exit the current process shall call `__cxa_finalize(NULL)`
      // before calling `ExitProcess()`.
      __MCF_thread_exit_callback();
    }
  }

#ifdef DLL_EXPORT

// When building the shared library, invoke the common routine from the DLL
// entry point callback.
int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  __asm__("__MCF_startup");

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    // The DLL shall not be loaded via `LoadLibrary()`.
    if((reason == DLL_PROCESS_ATTACH) && (reserved == NULL))
      return FALSE;

    __MCFGTHREAD_ASSERT(instance == &__my_image_base_from_gnu_ld);
    do_startup_common(reason);
    return TRUE;
  }

#else  // DLL_EXPORT

// When building the static library, invoke the common routine from a TLS
// callback. This requires the main executable be linked with 'tlssup.o'.
static void __stdcall
__MCF_tls_callback(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    (void) reserved;

    __MCFGTHREAD_ASSERT(instance == &__my_image_base_from_gnu_ld);
    do_startup_common(reason);
  }

static const PIMAGE_TLS_CALLBACK __MCF_xl_d
  __attribute__((__section__(".CRT$XLD"), __used__)) = __MCF_tls_callback;

#endif  // DLL_EXPORT
