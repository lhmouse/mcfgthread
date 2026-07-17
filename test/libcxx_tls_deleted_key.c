/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __libcpp_tls_key key;
static __libcpp_thread_t thrd;
static _MCF_sem thread_start = _MCF_SEM_INIT(0);
static _MCF_sem value_set = _MCF_SEM_INIT(0);
static _MCF_sem key_deleted = _MCF_SEM_INIT(0);
static int count;

static
void
tls_destructor(void* ptr)
  {
    (void) ptr;
    fprintf(stderr, "thread %d tls_destructor\n", __MCF_tid());
    _MCF_atomic_xadd_32_rlx(&count, 1);
  }

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&thread_start, NULL);

    int r = __libcpp_tls_set(key, &count);
    assert(r == 0);
    fprintf(stderr, "thread %d set value\n", __MCF_tid());

    _MCF_sem_signal(&value_set);
    _MCF_sem_wait(&key_deleted, NULL);

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    int r = __libcpp_tls_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    r = __libcpp_thread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting for value_set\n");
    _MCF_sem_signal(&thread_start);
    _MCF_sem_wait(&value_set, NULL);

    __libcpp_tls_delete(key);
    key = NULL;
    fprintf(stderr, "main deleted key; waiting for termination\n");
    _MCF_sem_signal(&key_deleted);

    __libcpp_thread_join(&thrd);
    fprintf(stderr, "main wait finished\n");

    assert(count == 0);
  }
