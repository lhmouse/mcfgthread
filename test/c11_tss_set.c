/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include <assert.h>
#include <stdio.h>

static tss_t key;
static thrd_t thrd;
static int dso_1, dso_2;

static
int
thread_proc(void* param)
  {
    (void) param;
    void* p;
    int r;

    p = tss_get(key);
    assert(p == __MCF_nullptr);

    r = tss_set(key, &dso_2);
    assert(r == thrd_success);

    p = tss_get(key);
    assert(p == &dso_2);

    _MCF_sleep((const int64_t[]) { -1001 });
    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    void* p;
    int r;

    r = tss_create(&key, __MCF_nullptr);
    assert(r == thrd_success);
    assert(key);

    p = tss_get(key);
    assert(p == __MCF_nullptr);

    r = tss_set(key, &dso_1);
    assert(r == thrd_success);

    p = tss_get(key);
    assert(p == &dso_1);

    r = thrd_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == thrd_success);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    thrd_join(thrd, __MCF_nullptr);
    fprintf(stderr, "main wait finished\n");

    p = tss_get(key);
    assert(p == &dso_1);
  }
