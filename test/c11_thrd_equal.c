/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/c11.h"
#include <assert.h>
#include <stdio.h>

static thrd_t thrd;

static
int
thread_proc(void* param)
  {
    (void) param;
    assert(thrd_equal(thrd_current(), thrd));

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    int r = thrd_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    assert(!thrd_equal(thrd_current(), thrd));

    printf("main waiting\n");
    r = thrd_join(thrd, __MCF_nullptr);
    assert(r == 0);
    printf("main wait finished\n");
  }
