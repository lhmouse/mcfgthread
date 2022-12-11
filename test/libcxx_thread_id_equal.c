/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include <assert.h>
#include <stdio.h>

static __libcpp_thread_t thrd;

static
void*
thread_proc(void* param)
  {
    (void) param;
    assert(__libcpp_thread_id_equal(__MCF_libcxx_thread_get_current_id(), __libcpp_thread_get_id(&thrd)));
    assert(__libcpp_thread_id_equal(__MCF_libcxx_thread_get_current_id(), _MCF_thread_self_tid()));

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
    int r = __libcpp_thread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    assert(!__libcpp_thread_id_equal(__MCF_libcxx_thread_get_current_id(), __libcpp_thread_get_id(&thrd)));

    printf("main waiting\n");
    r = __libcpp_thread_join(&thrd);
    assert(r == 0);
    printf("main wait finished\n");
  }
