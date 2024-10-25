/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/xglobals.h"
#include "../mcfgthread/exit.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

int unhandled_exit_code = 0;

static __MCF_NEVER_RETURN
LONG
__stdcall
unhandled(EXCEPTION_POINTERS* except)
  {
    (void) except;
    __MCF__Exit(unhandled_exit_code);
  }

static __MCF_NEVER_INLINE
void
test_top(void)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    fprintf(stderr, "raise exception 1\n");
    unhandled_exit_code = 42;
    RaiseException(0x20474343U, 0, 0, __MCF_nullptr);  // continue
    fprintf(stderr, "continue 1\n");
  }

static __MCF_NEVER_INLINE
void
test_unhandled(void)
  {
    fprintf(stderr, "raise exception 2\n");
    unhandled_exit_code = 0;
    RaiseException(0x20474343U, 0, 0, __MCF_nullptr);  // terminate
    fprintf(stderr, "continue 2\n");
    __MCF__Exit(41);
  }

static
DWORD
__stdcall
thread_proc(LPVOID param)
  {
    (void) param;
    test_top();
    test_unhandled();
    assert(false);
    return 1;
  }

int
main(void)
  {
    SetUnhandledExceptionFilter(unhandled);
    HANDLE thrd = CreateThread(__MCF_nullptr, 0, thread_proc, __MCF_nullptr, 0, __MCF_nullptr);
    assert(thrd);
    WaitForSingleObject(thrd, INFINITE);
    assert(false);
  }
