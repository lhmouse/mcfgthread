/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include <assert.h>
#include <stdio.h>

static thrd_t thrd;

static
int
thread_proc(void* param)
  {
    (void) param;
    assert(thrd_equal(thrd_current(), thrd));

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    int r = thrd_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    assert(!thrd_equal(thrd_current(), thrd));

    fprintf(stderr, "main waiting\n");
    r = thrd_join(thrd, __MCF_nullptr);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
