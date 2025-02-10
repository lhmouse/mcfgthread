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
static int magic;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sleep((const int64_t[]) { -500 });

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return &magic;
  }

int
main(void)
  {
    int r = __gthread_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    void* ret = __MCF_nullptr;
    r = __gthread_join(thrd, &ret);
    assert(r == 0);
    fprintf(stderr, "main wait finished: %p\n", ret);

    assert(ret == &magic);
  }
