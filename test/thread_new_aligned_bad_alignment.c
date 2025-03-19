/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
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
    _MCF_thread* thrd = _MCF_thread_new_aligned(thread_proc, 3, __MCF_nullptr, 0);
    assert(thrd == __MCF_nullptr);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    thrd = _MCF_thread_new_aligned(thread_proc, 0x40000000, __MCF_nullptr, 0);
    assert(thrd == __MCF_nullptr);
    assert(GetLastError() == ERROR_NOT_SUPPORTED);
  }
