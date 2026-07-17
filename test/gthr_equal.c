/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/gthr.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __gthread_t thrd;
static _MCF_mutex mtx = { 0 };

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_mutex_lock(&mtx, NULL);
    assert(__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    _MCF_mutex_lock(&mtx, NULL);
    int r = __gthread_create(&thrd, thread_proc, NULL);
    _MCF_mutex_unlock(&mtx);
    assert(r == 0);
    assert(thrd);

    assert(!__gthread_equal(__gthread_self(), thrd));

    fprintf(stderr, "main waiting\n");
    r = __gthread_join(thrd, NULL);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
