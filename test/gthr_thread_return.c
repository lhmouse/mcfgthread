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
static int magic;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sleep_noninterruptible(&(int64_t){ -500 });

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return &magic;
  }

int
main(void)
  {
    int r = __gthread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    void* ret = NULL;
    r = __gthread_join(thrd, &ret);
    assert(r == 0);
    fprintf(stderr, "main wait finished: %p\n", ret);

    assert(ret == &magic);
  }
