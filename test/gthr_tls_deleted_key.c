/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/sem.h"
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
    fprintf(stderr, "thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    _MCF_atomic_xadd_32_rlx(&count, 1);
  }

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&thread_start, __MCF_nullptr);

    int r = __gthread_setspecific(key, &count);
    assert(r == 0);
    fprintf(stderr, "thread %d set value\n", (int) _MCF_thread_self_tid());

    _MCF_sem_signal(&value_set);
    _MCF_sem_wait(&key_deleted, __MCF_nullptr);

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    int r = __gthread_key_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    r = __gthread_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == 0);
    assert(thrd);

    fprintf(stderr, "main waiting for value_set\n");
    _MCF_sem_signal(&thread_start);
    _MCF_sem_wait(&value_set, __MCF_nullptr);

    __gthread_key_delete(key);
    key = __MCF_nullptr;
    fprintf(stderr, "main deleted key; waiting for termination\n");
    _MCF_sem_signal(&key_deleted);

    __gthread_join(thrd, __MCF_nullptr);
    fprintf(stderr, "main wait finished\n");

    assert(count == 0);
  }
