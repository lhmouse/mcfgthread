/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include <assert.h>
#include <stdio.h>

static __gthread_t thrd;
static _MCF_mutex mtx = __MCF_0_INIT;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_mutex_lock(&mtx, __MCF_nullptr);
    assert(__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    _MCF_mutex_lock(&mtx, __MCF_nullptr);
    int r = __gthread_create(&thrd, thread_proc, __MCF_nullptr);
    _MCF_mutex_unlock(&mtx);
    assert(r == 0);
    assert(thrd);

    assert(!__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "main waiting\n");
    r = __gthread_join(thrd, __MCF_nullptr);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
