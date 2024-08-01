/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/once.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_once once;
static _MCF_once start;
static int resource = 0;

static int num_init = 0;   /* threads that performed initialization  */
static int num_ready = 0;  /* threads that saw so but didn't do it  */
static int num_timed_out = 0;

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_once_wait(&start, __MCF_nullptr);

    int r = _MCF_once_wait(&once, (const int64_t[]) { 0 });
    printf("thread %d got %d\n", self->__tid, r);
    if(r == 1) {
      /* Perform initialization.  */
      int old = resource;
      _MCF_sleep((const int64_t[]) { -200 });
      resource = old + 1;
      _MCF_once_release(&once);

      _MCF_sleep((const int64_t[]) { -100 });
      __atomic_fetch_add(&num_init, 1, __ATOMIC_RELAXED);
    }
    else if(r == 0) {
      /* Assume `resource` has been initialized.  */
      assert(resource == 1);

      _MCF_sleep((const int64_t[]) { -100 });
      __atomic_fetch_add(&num_ready, 1, __ATOMIC_RELAXED);
    }
    else if(r == -1) {
      _MCF_sleep((const int64_t[]) { -100 });
      __atomic_fetch_add(&num_timed_out, 1, __ATOMIC_RELAXED);
    }
    else
      assert(0);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, __MCF_nullptr, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_once_release(&start);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], __MCF_nullptr);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == 1);
    assert(num_init == 1);
    assert(num_ready + num_timed_out == NTHREADS - 1);
  }
