/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/libcxx.h"
#undef NDEBUG
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
    assert(p == NULL);

    r = __libcpp_tls_set(key, &dso_2);
    assert(r == 0);

    p = __libcpp_tls_get(key);
    assert(p == &dso_2);

    _MCF_sleep_noninterruptible(&(int64_t){ -1001 });
    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    void* p;
    int r;

    r = __libcpp_tls_create(&key, NULL);
    assert(r == 0);
    assert(key);

    p = __libcpp_tls_get(key);
    assert(p == NULL);

    r = __libcpp_tls_set(key, &dso_1);
    assert(r == 0);

    p = __libcpp_tls_get(key);
    assert(p == &dso_1);

    r = __libcpp_thread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    __libcpp_thread_join(&thrd);
    fprintf(stderr, "main wait finished\n");

    p = __libcpp_tls_get(key);
    assert(p == &dso_1);
  }
