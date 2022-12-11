/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/gthr.h"
#include <assert.h>
#include <stdio.h>

static __gthread_t thrd;

static
void*
thread_proc(void* param)
  {
    (void) param;
    assert(__gthread_equal(__gthread_self(), thrd));

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
    int r = __gthread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    assert(!__gthread_equal(__gthread_self(), thrd));

    printf("main waiting\n");
    r = __gthread_join(thrd, NULL);
    assert(r == 0);
    printf("main wait finished\n");
  }
