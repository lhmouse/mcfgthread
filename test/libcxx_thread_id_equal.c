/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/libcxx.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __libcpp_thread_t thrd;
static _MCF_mutex mtx = { 0 };

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_mutex_lock(&mtx, NULL);
    assert(__libcpp_thread_id_equal(__MCF_libcxx_thread_get_current_id(), __libcpp_thread_get_id(&thrd)));
    assert(__libcpp_thread_id_equal(__MCF_libcxx_thread_get_current_id(), _MCF_thread_self_tid()));

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    _MCF_mutex_lock(&mtx, NULL);
    int r = __libcpp_thread_create(&thrd, thread_proc, NULL);
    _MCF_mutex_unlock(&mtx);
    assert(r == 0);
    assert(thrd);

    assert(!__libcpp_thread_id_equal(__MCF_libcxx_thread_get_current_id(), __libcpp_thread_get_id(&thrd)));

    fprintf(stderr, "main waiting\n");
    r = __libcpp_thread_join(&thrd);
    assert(r == 0);
    fprintf(stderr, "main wait finished\n");
  }
