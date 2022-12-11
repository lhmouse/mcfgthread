/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __libcpp_thread_t threads[NTHREADS];
static __libcpp_tls_key key;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int count;

static
void
tls_destructor(void* ptr)
  {
    printf("thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    __atomic_fetch_add((int*) ptr, 1, __ATOMIC_RELAXED);
  }

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
    int r = __libcpp_tls_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = __libcpp_thread_create(&threads[k], thread_proc, NULL);
      assert(r == 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = __libcpp_thread_join(&threads[k]);
      assert(r == 0);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
