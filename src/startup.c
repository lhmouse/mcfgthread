// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_STARTUP_C_  1
#include "thread.h"
#include "win32.h"

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  __asm__("__MCF_startup");

extern char __my_image_base_from_gnu_ld
  __asm__("__image_base__");

void* const _MCF_crt_module = &__my_image_base_from_gnu_ld;
uint32_t _MCF_tls_index;
double _MCF_perf_frequency_reciprocal;
_MCF_thread _MCF_main_thread;

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
      _MCF_tls_index = TlsAlloc();
      __MCFGTHREAD_CHECK(_MCF_tls_index != TLS_OUT_OF_INDEXES);

      // Get the performance counter resolution.
      LARGE_INTEGER freq;
      __MCFGTHREAD_CHECK(QueryPerformanceFrequency(&freq));
      _MCF_perf_frequency_reciprocal = 1000 / (double) freq.QuadPart;

      // Attach the main thread.
      _MCF_main_thread.__tid = GetCurrentThreadId();
      _MCF_main_thread.__handle = OpenThread(THREAD_ALL_ACCESS, FALSE, _MCF_main_thread.__tid);
      __MCFGTHREAD_CHECK(_MCF_main_thread.__handle);
      __atomic_store_n(_MCF_main_thread.__nref, 1, __ATOMIC_RELEASE);
      TlsSetValue(_MCF_tls_index, &_MCF_main_thread);
    }
    else if(reason == DLL_THREAD_DETACH) {
      // Perform per-thread cleanup.
      // This is not done at process detach as it is too late.
      __MCF_thread_exit_callback();
    }
    return TRUE;
  }
