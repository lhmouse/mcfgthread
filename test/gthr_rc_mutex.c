/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __gthread_t threads[NTHREADS];
static __gthread_recursive_mutex_t mutex = __GTHREAD_RECURSIVE_MUTEX_INIT;
static _MCF_sem start = _MCF_SEM_INIT(0);
static int resource = 0;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    int r = __gthread_recursive_mutex_lock(&mutex);
    assert(r == 0);
    r = __gthread_recursive_mutex_lock(&mutex);
    assert(r == 0);
    r = __gthread_recursive_mutex_lock(&mutex);
    assert(r == 0);

    /* Add a resource.  */
    int old = resource;
    _MCF_sleep_noninterruptible(&(int64_t){ -10 });
    resource = old + 1;

    r = __gthread_recursive_mutex_unlock(&mutex);
    assert(r == 0);
    r = __gthread_recursive_mutex_unlock(&mutex);
    assert(r == 0);
    r = __gthread_recursive_mutex_unlock(&mutex);
    assert(r == 0);

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return NULL;
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_create(&threads[k], thread_proc, NULL);
      assert(r == 0);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_join(threads[k], NULL);
      assert(r == 0);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
