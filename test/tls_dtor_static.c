/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_tls_key* key;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int count;

static
void
tls_destructor(void* ptr)
  {
    printf("thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    __atomic_fetch_add((int*) ptr, 1, __ATOMIC_RELAXED);
  }

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&start, __MCF_nullptr);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);
    assert(_MCF_tls_key_get_destructor(key) == tls_destructor);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, __MCF_nullptr, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], __MCF_nullptr);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
