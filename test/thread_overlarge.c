/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    assert(_MCF_thread_new(thread_proc, __MCF_nullptr, PTRDIFF_MAX) == __MCF_nullptr);
    assert(GetLastError() == ERROR_ARITHMETIC_OVERFLOW);
  }
