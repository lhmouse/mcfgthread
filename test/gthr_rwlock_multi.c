/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  16U
static __gthread_t reader_threads[NTHREADS];
static __gthread_t writer_threads[NTHREADS];
static __MCF_ALIGNED(128) __gthread_rwlock_t mutex = __GTHREAD_RWLOCK_INIT;
static __MCF_ALIGNED(128) _MCF_sem start = _MCF_SEM_INIT(0);

#define NITER  10000U
static __MCF_ALIGNED(128) int res1 = 0;
static __MCF_ALIGNED(128) int res2 = 0;

static
void*
reader_thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    for(uint32_t i = 0; i < NITER;  ++i) {
      int r = __gthread_rwlock_rdlock(&mutex);
      assert(r == 0);

      /* Check resources are equal.  */
      int t1 = res1;
      __gthread_yield();
      assert(t1 == res2);
      __gthread_rwlock_unlock(&mutex);

      //fprintf(stderr, "R    thread %d\n", __MCF_tid());
      __gthread_yield();
    }

    fprintf(stderr, "R    thread %d quitting\n", __MCF_tid());
    return NULL;
  }

static
void*
writer_thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    for(uint32_t i = 0; i < NITER;  ++i) {
      int r = __gthread_rwlock_wrlock(&mutex);
      assert(r == 0);

      /* Add resources.  */
      res1 ++;
      __gthread_yield();
      res2 ++;
      assert(res1 == res2);
      __gthread_rwlock_unlock(&mutex);

      //fprintf(stderr, "  W  thread %d\n", __MCF_tid());
      __gthread_yield();
    }

    fprintf(stderr, "  W  thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_create(&reader_threads[k], reader_thread_proc, NULL);
      assert(r == 0);
      assert(reader_threads[k]);
      r = __gthread_create(&writer_threads[k], writer_thread_proc, NULL);
      assert(r == 0);
      assert(writer_threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS * 2);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_join(reader_threads[k], NULL);
      assert(r == 0);
      r = __gthread_join(writer_threads[k], NULL);
      assert(r == 0);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(res1 == NITER * NTHREADS);
    assert(res2 == NITER * NTHREADS);
  }
