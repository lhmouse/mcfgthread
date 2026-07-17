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
static tss_t key;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int count;

static
void
tls_destructor(void* ptr)
  {
    fprintf(stderr, "thread %d tls_destructor\n", __MCF_tid());
    _MCF_atomic_xadd_32_rlx(ptr, 1);
  }

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    int r = _MCF_tls_set(key, &count);
    assert(r == thrd_success);

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return 0;
  }

int
main(void)
  {
    int r = tss_create(&key, tls_destructor);
    assert(r == thrd_success);
    assert(key);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = thrd_create(&threads[k], thread_proc, NULL);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = thrd_join(threads[k], NULL);
      assert(r == thrd_success);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
