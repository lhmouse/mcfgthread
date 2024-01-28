/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include <assert.h>
#include <stdio.h>

static __libcpp_tls_key key;
static __libcpp_thread_t thrd;
static int dso_1, dso_2;

static
void*
thread_proc(void* param)
  {
    (void) param;
    void* p;
    int r;

    p = __libcpp_tls_get(key);
    assert(p == __MCF_nullptr);

    r = __libcpp_tls_set(key, &dso_2);
    assert(r == 0);

    p = __libcpp_tls_get(key);
    assert(p == &dso_2);

    _MCF_sleep((const int64_t[]) { -1000 });
    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    void* p;
    int r;

    r = __libcpp_tls_create(&key, __MCF_nullptr);
    assert(r == 0);
    assert(key);

    p = __libcpp_tls_get(key);
    assert(p == __MCF_nullptr);

    r = __libcpp_tls_set(key, &dso_1);
    assert(r == 0);

    p = __libcpp_tls_get(key);
    assert(p == &dso_1);

    r = __libcpp_thread_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    printf("main waiting\n");
    __libcpp_thread_join(&thrd);
    printf("main wait finished\n");

    p = __libcpp_tls_get(key);
    assert(p == &dso_1);
  }
