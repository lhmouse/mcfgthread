/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/once.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_once once;
static _MCF_once start;
static int resource = 0;

static int num_init = 0;   /* threads that performed initialization  */
static int num_ready = 0;  /* threads that saw so but didn't do it  */

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_once_wait(&start, NULL);

    int r = _MCF_once_wait(&once, NULL);
    printf("thread %d got %d\n", self->__tid, r);
    if(r == 1) {
      /* Perform initialization.  */
      int old = resource;
      int64_t sleep_time = -10;
      _MCF_sleep(&sleep_time);
      resource = old + 1;
      _MCF_once_abort(&once);

      sleep_time = -100;
      _MCF_sleep(&sleep_time);
      __atomic_fetch_add(&num_init, 1, __ATOMIC_RELAXED);
    }
    else if(r == 0) {
      /* Assume `resource` has been initialized.  */
      assert(resource == 1);

      int64_t sleep_time = -100;
      _MCF_sleep(&sleep_time);
      __atomic_fetch_add(&num_ready, 1, __ATOMIC_RELAXED);
    }
    else
      assert(0);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_once_release(&start);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], NULL);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
    assert(num_init == NTHREADS);
    assert(num_ready == 0);
  }
