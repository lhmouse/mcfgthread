/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_EXIT(...)  __MCF_DLLEXPORT
#include "exit.h"
#include "xglobals.i"

__MCF_DLLEXPORT
void
__MCF__Exit(int status)
  {
    /* Terminte the current process without invoking TLS callbacks.  */
    TerminateProcess(GetCurrentProcess(), (DWORD) status);
    __MCF_UNREACHABLE;
  }

__MCF_DLLEXPORT
void
__MCF_quick_exit(int status)
  {
    /* Invoke all callbacks that have been registered by `at_quick_exit()` in
     * reverse order.  */
    __MCF_run_dtors_at_quick_exit();
    __MCF__Exit(status);
  }

__MCF_DLLEXPORT
void
__MCF_exit(int status)
  {
    /* Perform global cleanup like `__cxa_finalize(NULL)`.  */
    __MCF_finalize_on_exit();
    __MCF__Exit(status);
  }
