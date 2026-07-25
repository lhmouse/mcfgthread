/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_EXIT_IMPORT  __MCF_DLLEXPORT
#define __MCF_EXIT_INLINE  __MCF_DLLEXPORT
#include "../exit.h"
#include "../cxa.h"
#include "xglobals.h"

__MCF_DLLEXPORT
void
__MCF__Exit(int status)
  {
    TerminateProcess(NtCurrentProcess(), (ULONG) status);
    __builtin_trap();
  }

__MCF_DLLEXPORT
void
__MCF_quick_exit(int status)
  {
    __MCF_run_static_dtors(__MCF_G(quick_exit_mtx), __MCF_G(quick_exit_queue), nullptr);
    __MCF__Exit(status);
  }

__MCF_DLLEXPORT
void
__MCF_exit(int status)
  {
    __MCF_cxa_finalize(nullptr);
    __MCF__Exit(status);
  }
