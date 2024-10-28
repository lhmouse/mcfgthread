/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include <assert.h>
#include <stdio.h>

static __gthread_key_t key;
static __gthread_t thrd;
static int dso_1, dso_2;

static
void*
thread_proc(void* param)
  {
    (void) param;
    void* p;
    int r;

    p = __gthread_getspecific(key);
    assert(p == __MCF_nullptr);

    r = __gthread_setspecific(key, &dso_2);
    assert(r == 0);

    p = __gthread_getspecific(key);
    assert(p == &dso_2);

    _MCF_sleep((const int64_t[]) { -1001 });
    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    void* p;
    int r;

    r = __gthread_key_create(&key, __MCF_nullptr);
    assert(r == 0);
    assert(key);

    p = __gthread_getspecific(key);
    assert(p == __MCF_nullptr);

    r = __gthread_setspecific(key, &dso_1);
    assert(r == 0);

    p = __gthread_getspecific(key);
    assert(p == &dso_1);

    r = __gthread_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    __gthread_join(thrd, __MCF_nullptr);
    fprintf(stderr, "main wait finished\n");

    p = __gthread_getspecific(key);
    assert(p == &dso_1);
  }
