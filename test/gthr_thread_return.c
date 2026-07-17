/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

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
