/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/libcxx.h"
#include "../src/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __libcpp_thread_t threads[NTHREADS];
static __libcpp_recursive_mutex_t mutex;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    int r = __libcpp_recursive_mutex_lock(&mutex);
    assert(r == 0);
    r = __libcpp_recursive_mutex_lock(&mutex);
    assert(r == 0);
    r = __libcpp_recursive_mutex_lock(&mutex);
    assert(r == 0);

    /* Add a resource.  */
    int old = resource;
    _MCF_sleep((const int64_t[]) { -10 });
    resource = old + 1;

    r = __libcpp_recursive_mutex_unlock(&mutex);
    assert(r == 0);
    r = __libcpp_recursive_mutex_unlock(&mutex);
    assert(r == 0);
    r = __libcpp_recursive_mutex_unlock(&mutex);
    assert(r == 0);

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
    int init = __libcpp_recursive_mutex_init(&mutex);
    assert(init == 0);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __libcpp_thread_create(&threads[k], thread_proc, NULL);
      assert(r == 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __libcpp_thread_join(&threads[k]);
      assert(r == 0);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
