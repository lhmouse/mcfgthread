/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static thrd_t thrd;
static int magic = 123456789;

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sleep_noninterruptible(&(int64_t){ -500 });

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return magic;
  }

int
main(void)
  {
    int r = thrd_create(&thrd, thread_proc, NULL);
    assert(r == thrd_success);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    int ret = 6543210;
    r = thrd_join(thrd, &ret);
    assert(r == thrd_success);
    fprintf(stderr, "main wait finished: %d\n", ret);

    assert(ret == magic);
  }
