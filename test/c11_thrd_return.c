/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include <assert.h>
#include <stdio.h>

static thrd_t thrd;
static int magic = 123456789;

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sleep((const int64_t[]) { -500 });

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return magic;
  }

int
main(void)
  {
    int r = thrd_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == thrd_success);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    int ret = 6543210;
    r = thrd_join(thrd, &ret);
    assert(r == thrd_success);
    fprintf(stderr, "main wait finished: %d\n", ret);

    assert(ret == magic);
  }
