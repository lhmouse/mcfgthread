/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
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
    assert(p == NULL);

    r = tss_set(key, &dso_2);
    assert(r == thrd_success);

    p = tss_get(key);
    assert(p == &dso_2);

    _MCF_sleep((const int64_t[]) { -1000 });
    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    void* p;
    int r;

    r = tss_create(&key, NULL);
    assert(r == thrd_success);
    assert(key);

    p = tss_get(key);
    assert(p == NULL);

    r = tss_set(key, &dso_1);
    assert(r == thrd_success);

    p = tss_get(key);
    assert(p == &dso_1);

    r = thrd_create(&thrd, thread_proc, NULL);
    assert(r == thrd_success);
    assert(thrd);

    printf("main waiting\n");
    thrd_join(thrd, NULL);
    printf("main wait finished\n");

    p = tss_get(key);
    assert(p == &dso_1);
  }
