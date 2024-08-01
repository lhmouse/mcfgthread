/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

static tss_t key;
static thrd_t thrd;
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
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&thread_start, __MCF_nullptr);

    int r = tss_set(key, &count);
    assert(r == thrd_success);
    printf("thread %d set value\n", (int) _MCF_thread_self_tid());

    _MCF_sem_signal(&value_set);
    _MCF_sem_wait(&key_deleted, __MCF_nullptr);

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    int r = tss_create(&key, tls_destructor);
    assert(r == thrd_success);
    assert(key);

    r = thrd_create(&thrd, thread_proc, __MCF_nullptr);
    assert(r == thrd_success);
    assert(thrd);

    printf("main waiting for value_set\n");
    _MCF_sem_signal(&thread_start);
    _MCF_sem_wait(&value_set, __MCF_nullptr);

    tss_delete(key);
    key = __MCF_nullptr;
    printf("main deleted key; waiting for termination\n");
    _MCF_sem_signal(&key_deleted);

    thrd_join(thrd, __MCF_nullptr);
    printf("main wait finished\n");

    assert(count == 0);
  }
