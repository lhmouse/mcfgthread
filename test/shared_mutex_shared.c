/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/shared_mutex.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_shared_mutex mutex;
static _MCF_sem start = _MCF_SEM_INIT(0);
static int resource = 0;

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&start, NULL);

    int r = _MCF_shared_mutex_lock_shared(&mutex, NULL);
    assert(r == 0);

    /* Add a resource.  */
    int old = _MCF_atomic_load_32_rlx(&resource);
    _MCF_sleep_noninterruptible(&(int64_t){ -10 });
    _MCF_atomic_store_32_rlx(&resource, old + 1);
    _MCF_shared_mutex_unlock(&mutex);

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

    assert(resource < (int) NTHREADS);
  }
