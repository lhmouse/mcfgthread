/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/libcxx.h"
#include <assert.h>
#include <stdio.h>

static __libcpp_thread_t thrd;
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
    int r = __libcpp_thread_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    r = __libcpp_thread_join(&thrd);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
