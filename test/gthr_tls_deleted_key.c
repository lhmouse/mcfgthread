/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include "../src/sem.h"
#include <assert.h>
#include <stdio.h>

static __gthread_key_t key;
static __gthread_t thrd;
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
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&thread_start, NULL);

    int r = __gthread_setspecific(key, &count);
    assert(r == 0);
    printf("thread %d set value\n", (int) _MCF_thread_self_tid());

    _MCF_sem_signal_some(&value_set, 1);
    _MCF_sem_wait(&key_deleted, NULL);

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
    int r = __gthread_key_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    r = __gthread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    printf("main waiting for value_set\n");
    _MCF_sem_signal_some(&thread_start, 1);
    _MCF_sem_wait(&value_set, NULL);

    __gthread_key_delete(key);
    key = NULL;
    printf("main deleted key; waiting for termination\n");
    _MCF_sem_signal_some(&key_deleted, 1);

    __gthread_join(thrd, NULL);
    printf("main wait finished\n");

    assert(count == 0);
  }
