/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_tls_key* key;
static _MCF_thread* thrd;
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
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&thread_start, NULL);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);
    fprintf(stderr, "thread %d set value\n", self->__tid);

    _MCF_sem_signal(&value_set);
    _MCF_sem_wait(&key_deleted, NULL);

    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);

    thrd = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thrd);

    fprintf(stderr, "main waiting for value_set\n");
    _MCF_sem_signal(&thread_start);
    _MCF_sem_wait(&value_set, NULL);

    _MCF_tls_key_delete(key);
    key = NULL;
    fprintf(stderr, "main deleted key; waiting for termination\n");
    _MCF_sem_signal(&key_deleted);

    _MCF_thread_wait(thrd, NULL);
    fprintf(stderr, "main wait finished\n");

    assert(count == 0);
  }
