/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/xglobals.h"
#include "../mcfgthread/exit.h"
#include <windows.h>
#include <stdio.h>

int unhandled_exit_code;

static __MCF_NEVER_RETURN
LONG
__stdcall
unhandled(EXCEPTION_POINTERS* except)
  {
    (void) except;
    __MCF__Exit(unhandled_exit_code);
  }

int
main(void)
  {
    SetUnhandledExceptionFilter(unhandled);

    {
      /* Raise `(1 << 29) | 'GCC'` and expect continuation.  */
      __MCF_SEH_DEFINE_TERMINATE_FILTER;
      fprintf(stderr, "raise exception 1\n");
      unhandled_exit_code = 42;
      RaiseException(0x20474343U, 0, 0, __MCF_nullptr);
    }

    {
      /* Raise `(1 << 29) | 'GCC'` and expect termination.  */
      fprintf(stderr, "raise exception 2\n");
      unhandled_exit_code = 0;
      RaiseException(0x20474343U, 0, 0, __MCF_nullptr);
      __MCF__Exit(41);
    }
  }
