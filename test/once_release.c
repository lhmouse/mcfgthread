/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/once.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_once once;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int resource = 0;

static int num_init = 0;   /* threads that performed initialization  */
static int num_ready = 0;  /* threads that saw so but didn't do it  */

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&start, NULL);

    int r = _MCF_once_wait(&once, NULL);
    fprintf(stderr, "thread %d got %d\n", self->__tid, r);
    if(r == 1) {
      /* Perform initialization.  */
      int old = resource;
      _MCF_sleep_noninterruptible(&(int64_t){ -200 });
      resource = old + 1;
      _MCF_once_release(&once);

      _MCF_sleep_noninterruptible(&(int64_t){ -100 });
      _MCF_atomic_xadd_32_rlx(&num_init, 1);
    }
    else if(r == 0) {
      /* Assume `resource` has been initialized.  */
      assert(resource == 1);

      _MCF_sleep_noninterruptible(&(int64_t){ -100 });
      _MCF_atomic_xadd_32_rlx(&num_ready, 1);
    }
    else
      assert(0);

    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], NULL);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == 1);
    assert(num_init == 1);
    assert(num_ready == NTHREADS - 1);
  }
