/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_EXIT_IMPORT  __MCF_DLLEXPORT
#define __MCF_EXIT_INLINE  __MCF_DLLEXPORT
#include "exit.h"
#include "dtor_queue.h"
#include "cxa.h"
#include "xglobals.i"

__MCF_DLLEXPORT
void
__MCF__Exit(int status)
  {
    TerminateProcess(GetCurrentProcess(), (DWORD) status);
    __MCF_UNREACHABLE;
  }

__MCF_DLLEXPORT
void
__MCF_quick_exit(int status)
  {
    __MCF_dtor_queue_finalize(__MCF_g->__cxa_at_quick_exit_queue, __MCF_g->__cxa_at_quick_exit_mtx, NULL);
    __MCF__Exit(status);
  }

__MCF_DLLEXPORT
void
__MCF_exit(int status)
  {
    __MCF_cxa_finalize(NULL);
    __MCF__Exit(status);
  }
