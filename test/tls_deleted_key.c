/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include "../src/sem.h"
#include <assert.h>
#include <stdio.h>

static _MCF_tls_key* key;
static _MCF_thread* thrd;
static _MCF_sem thread_start = __MCF_SEM_INIT(0);
static _MCF_sem value_set = __MCF_SEM_INIT(0);
static _MCF_sem key_deleted = __MCF_SEM_INIT(0);
static int count;

static
void
tls_destructor(void* ptr)
  {
    (void) ptr;
    printf("thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    __atomic_fetch_add(&count, 1, __ATOMIC_RELAXED);
  }

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&thread_start, NULL);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);
    printf("thread %d set value\n", self->__tid);

    _MCF_sem_signal_some(&value_set, 1);
    _MCF_sem_wait(&key_deleted, NULL);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);

    thrd = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thrd);

    printf("main waiting for value_set\n");
    _MCF_sem_signal_some(&thread_start, 1);
    _MCF_sem_wait(&value_set, NULL);

    _MCF_tls_key_delete(key);
    key = NULL;
    printf("main deleted key; waiting for termination\n");
    _MCF_sem_signal_some(&key_deleted, 1);

    _MCF_thread_wait(thrd, NULL);
    printf("main wait finished\n");

    assert(count == 0);
  }
