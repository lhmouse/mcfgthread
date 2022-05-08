/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#include "fwd.h"
#include "xwin32.i"

static
void __stdcall
do_image_tls_callback(PVOID module, DWORD reason, LPVOID reserved)
  {
    (void) module;
    (void) reserved;

    /* Perform global initialization and per-thread cleanup, as needed.
     * Note, upon `DLL_PROCESS_DETACH` we don't perform any cleanup, because
     * other DLLs might have been unloaded and we would be referencing unmapped
     * memory. User code should call `__cxa_finalize(NULL)` before exiting from
     * a process.  */
    if(reason == DLL_PROCESS_ATTACH)
      __MCF_dll_callback_on_process_attach();
    else if(reason == DLL_THREAD_DETACH)
      __MCF_dll_callback_on_thread_detach();
  }

#ifdef DLL_EXPORT

__MCF_DLLEXPORT
/* When building the shared library, invoke the common routine from the DLL
 * entry point callback. The decorated name is fabricated such that it remains
 * the same on both x86 and x86-64.  */
int __stdcall
__MCF_dll_startup(PVOID instance, DWORD reason, PVOID reserved)
  __asm__("__MCF_dll_startup@@Z");

__MCF_DLLEXPORT
int __stdcall
__MCF_dll_startup(PVOID instance, DWORD reason, PVOID reserved)
  {
    if(reason == DLL_PROCESS_ATTACH) {
      /* Prevent this DLL from being unloaded.  */
      HMODULE locked;
      __MCFGTHREAD_CHECK(GetModuleHandleExW(
          GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
          (void*) instance, &locked));
    }

    /* Call the common routine. This will not fail.  */
    do_image_tls_callback(instance, reason, reserved);
    return 1;
  }

#else  /* DLL_EXPORT  */

/* When building the static library, invoke the common routine from a TLS
 * callback. This requires the main executable be linked with 'tlssup.o'.
 * `__xl_d` is being used by mingw-w64, so we use `__xl_e` here.  */
extern const PIMAGE_TLS_CALLBACK __MCF_xl_e;

const PIMAGE_TLS_CALLBACK __MCF_xl_e
  __attribute__((__section__(".CRT$XLE"), __used__)) = do_image_tls_callback;

#endif  /* DLL_EXPORT  */
