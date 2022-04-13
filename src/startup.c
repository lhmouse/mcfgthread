// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_STARTUP_C_  1
#include "thread.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "win32.h"

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  __asm__("__MCF_startup");

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

// Initialize global variables. This function is called by the DLL startup
// routine or the TLS callback from the main executable.
static void
__MCF_crt_initialize(void)
  {
    // Create the CRT heap for memory allocation.
    __MCFGTHREAD_ASSERT(!__MCF_crt_heap);
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

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    __MCFGTHREAD_ASSERT(instance == _MCF_crt_module);

    switch(reason) {
      case DLL_PROCESS_ATTACH:
        // Fail if this library is loaded dynamically.
        // Also note that no cleanup is performed upon `DLL_PROCESS_DETACH`.
        if(reserved == NULL)
          return FALSE;

        __MCF_crt_initialize();
        return TRUE;

      case DLL_THREAD_DETACH:
        // Perform per-thread cleanup.
        // This is not done at process detach as it is too late. A thread that
        // wishes to exit the current process shall call `__cxa_finalize(NULL)`
        // before calling `ExitProcess()`.
        __MCF_thread_exit_callback();
        return TRUE;

      default:
        return TRUE;
    }
  }
