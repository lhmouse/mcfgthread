/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#undef NDEBUG
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

    _MCF_sleep_noninterruptible(&(int64_t){ -1001 });
    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
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

    fprintf(stderr, "main waiting\n");
    thrd_join(thrd, NULL);
    fprintf(stderr, "main wait finished\n");

    p = tss_get(key);
    assert(p == &dso_1);
  }
