/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static tss_t key;
static thrd_t thrd;
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
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&thread_start, NULL);

    int r = tss_set(key, &count);
    assert(r == thrd_success);
    fprintf(stderr, "thread %d set value\n", __MCF_tid());

    _MCF_sem_signal(&value_set);
    _MCF_sem_wait(&key_deleted, NULL);

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return 0;
  }

int
main(void)
  {
    int r = tss_create(&key, tls_destructor);
    assert(r == thrd_success);
    assert(key);

    r = thrd_create(&thrd, thread_proc, NULL);
    assert(r == thrd_success);
    assert(thrd);

    fprintf(stderr, "main waiting for value_set\n");
    _MCF_sem_signal(&thread_start);
    _MCF_sem_wait(&value_set, NULL);

    tss_delete(key);
    key = NULL;
    fprintf(stderr, "main deleted key; waiting for termination\n");
    _MCF_sem_signal(&key_deleted);

    thrd_join(thrd, NULL);
    fprintf(stderr, "main wait finished\n");

    assert(count == 0);
  }
