/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>

static __gthread_t thrd;
static int magic;

static
void*
thread_proc(void* param)
  {
    (void) param;
    int64_t sleep_time = -500;
    _MCF_sleep(&sleep_time);

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return &magic;
  }

int
main(void)
  {
    int r = __gthread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    printf("main waiting\n");
    void* ret;
    r = __gthread_join(thrd, &ret);
    assert(r == 0);
    printf("main wait finished: %p\n", ret);

    assert(ret == &magic);
  }
