/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include <assert.h>
#include <stdio.h>

static __gthread_t thrd;

static
void*
thread_proc(void* param)
  {
    (void) param;
    assert(__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    int r = __gthread_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    assert(!__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "main waiting\n");
    r = __gthread_join(thrd, __MCF_nullptr);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
