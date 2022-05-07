/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_EXIT_EXTERN_INLINE
#include "exit.h"
#include "xwin32.i"

void
__MCF__Exit(int status)
  {
    /* Terminte the current process without invoking TLS callbacks.  */
    TerminateProcess(GetCurrentProcess(), (DWORD) status);
    __MCF_UNREACHABLE;
  }

void
__MCF_quick_exit(int status)
  {
    /* Invoke all callbacks that have been registered by `at_quick_exit()` in
     * reverse order.  */
    __MCF_run_dtors_at_quick_exit();

    /* Call `_Exit(status)` in accordance with the ISO C standard.  */
    __MCF__Exit(status);
  }

void
__MCF_exit(int status)
  {
    /* Perform global cleanup like `__cxa_finalize(NULL)`.  */
    __MCF_finalize_on_exit();

    /* After the CRT has been finalized, exit.  */
    __MCF__Exit(status);
  }
