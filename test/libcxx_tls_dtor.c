/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __libcpp_thread_t threads[NTHREADS];
static __libcpp_tls_key key;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int count;

static
void
tls_destructor(void* ptr)
  {
    fprintf(stderr, "thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    _MCF_atomic_xadd_32_rlx(ptr, 1);
  }

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, __MCF_nullptr);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    int r = __libcpp_tls_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = __libcpp_thread_create(&threads[k], thread_proc, __MCF_nullptr);
      assert(r == 0);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = __libcpp_thread_join(&threads[k]);
      assert(r == 0);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
