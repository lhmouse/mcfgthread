/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __gthread_t thrd;
static _MCF_mutex mtx = { 0 };

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_mutex_lock(&mtx, NULL);
    assert(__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    _MCF_mutex_lock(&mtx, NULL);
    int r = __gthread_create(&thrd, thread_proc, NULL);
    _MCF_mutex_unlock(&mtx);
    assert(r == 0);
    assert(thrd);

    assert(!__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "main waiting\n");
    r = __gthread_join(thrd, NULL);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
