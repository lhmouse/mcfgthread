// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCF_STARTUP  1
#include "thread.h"
#include "win32.h"

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  __asm__("__MCF_startup");

extern char __my_image_base_from_gnu_ld
  __asm__("__image_base__");

void* const _MCF_crt_module = &__my_image_base_from_gnu_ld;
uint32_t _MCF_tls_index = 0;
_MCF_thread_control _MCF_main_thread = { 0 };

int __stdcall
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    if(instance != &__my_image_base_from_gnu_ld)
      DebugBreak();

    if(reason == DLL_PROCESS_ATTACH) {
      // Fail if this library is loaded dynamically.
      // Also note that no cleanup is performed upon `DLL_PROCESS_DETACH`.
      if(reserved == NULL)
        return false;

      // Allocate a TLS slot for this library.
      _MCF_tls_index = TlsAlloc();
      if(_MCF_tls_index == TLS_OUT_OF_INDEXES)
        DebugBreak();

      // Attach the main thread.
      _MCF_main_thread.__handle = OpenThread(THREAD_ALL_ACCESS, false, _MCF_main_thread.__tid);
      if(_MCF_main_thread.__handle == NULL)
        DebugBreak();

      _MCF_main_thread.__tid = GetCurrentThreadId();
      __atomic_store_n(_MCF_main_thread.__nref, -1, __ATOMIC_RELAXED);
      TlsSetValue(_MCF_tls_index, &_MCF_main_thread);
    }
    else if(reason == DLL_THREAD_DETACH) {
      // Perform per-thread cleanup.
      // This is not done at process detach as it is too late.
      __MCF_thread_exit_callback();
    }
    return true;
  }
