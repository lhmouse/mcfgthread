// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_STARTUP_C_  1
#include "thread.h"
#include "mutex.h"
#include "dtorque.h"
#include "win32.h"

uint32_t __MCF_tls_index;
double __MCF_perf_frequency_reciprocal;
_MCF_thread __MCF_main_thread;

_MCF_mutex __MCF_cxa_atexit_mutex;
__MCF_dtorque __MCF_cxa_atexit_queue;

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  __asm__("__MCF_startup");

extern char __my_image_base_from_gnu_ld
  __asm__("__image_base__");

void* const _MCF_crt_module = &__my_image_base_from_gnu_ld;

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    __MCFGTHREAD_ASSERT(instance == &__my_image_base_from_gnu_ld);

    if(reason == DLL_PROCESS_ATTACH) {
      // Fail if this library is loaded dynamically.
      // Also note that no cleanup is performed upon `DLL_PROCESS_DETACH`.
      if(reserved == NULL)
        return FALSE;

      // Allocate a TLS slot for this library.
      __MCF_tls_index = TlsAlloc();
      __MCFGTHREAD_CHECK(__MCF_tls_index != TLS_OUT_OF_INDEXES);

      // Get the performance counter resolution.
      LARGE_INTEGER freq;
      __MCFGTHREAD_CHECK(QueryPerformanceFrequency(&freq));
      __MCF_perf_frequency_reciprocal = 1000 / (double) freq.QuadPart;

      // Attach the main thread.
      __MCF_main_thread.__tid = GetCurrentThreadId();
      __MCF_main_thread.__handle = OpenThread(THREAD_ALL_ACCESS, FALSE, __MCF_main_thread.__tid);
      __MCFGTHREAD_CHECK(__MCF_main_thread.__handle);
      __atomic_store_n(__MCF_main_thread.__nref, 1, __ATOMIC_RELEASE);
      __MCFGTHREAD_CHECK(TlsSetValue(__MCF_tls_index, &__MCF_main_thread));
    }
    else if(reason == DLL_THREAD_DETACH) {
      // Perform per-thread cleanup.
      // This is not done at process detach as it is too late.
      __MCF_thread_exit_callback();
    }
    return TRUE;
  }
