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
static once_flag once = ONCE_FLAG_INIT;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int resource = 0;

static
void
once_do_it(void)
  {
    /* Perform initialization.  */
    int old = resource;
    _MCF_sleep_noninterruptible(&(int64_t){ -200 });
    resource = old + 1;

    _MCF_sleep_noninterruptible(&(int64_t){ -100 });
    fprintf(stderr, "thread %d once\n", __MCF_tid());
  }

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    call_once(&once, once_do_it);
    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return 0;
  }

int
main(void)
  {
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

    assert(resource == 1);
  }
