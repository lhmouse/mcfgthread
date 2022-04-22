/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>

static __gthread_key_t key;
static __gthread_t thrd;
static __gthread_once_t thread_start;
static __gthread_once_t value_set;
static __gthread_once_t key_deleted;
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
    _MCF_once_wait(&thread_start, NULL);
    _MCF_once_wait(&thread_start, NULL);

    int r = __gthread_setspecific(key, &count);
    assert(r == 0);
    printf("thread %d set value\n", (int) _MCF_thread_self_tid());

    _MCF_once_release(&value_set);
    _MCF_once_wait(&key_deleted, NULL);
    _MCF_once_wait(&key_deleted, NULL);

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
    _MCF_once_release(&thread_start);
    _MCF_once_wait(&value_set, NULL);
    _MCF_once_wait(&value_set, NULL);

    __gthread_key_delete(key);
    key = NULL;
    printf("main deleted key; waiting for termination\n");
    _MCF_once_release(&key_deleted);

    __gthread_join(thrd, NULL);
    printf("main wait finished\n");

    assert(count == 0);
  }
