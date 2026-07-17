/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];
static mtx_t mutex;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int resource = 0;

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    int r = mtx_lock(&mutex);
    assert(r == thrd_success);

    /* Add a resource.  */
    int old = resource;
    _MCF_sleep_noninterruptible(&(int64_t){ -10 });
    resource = old + 1;
    mtx_unlock(&mutex);

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return 0;
  }

int
main(void)
  {
    int err = mtx_init(&mutex, mtx_plain);
    assert(err == thrd_success);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, NULL);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], NULL);
      assert(r == thrd_success);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
