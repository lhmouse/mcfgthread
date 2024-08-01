/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __gthread_t threads[NTHREADS];
static __gthread_recursive_mutex_t mutex = __GTHREAD_RECURSIVE_MUTEX_INIT;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, __MCF_nullptr);

    for(;;) {
      int r = __gthread_recursive_mutex_trylock(&mutex);
      if(r == 0) {
        printf("thread %d got %d\n", (int) _MCF_thread_self_tid(), r);

        r = __gthread_recursive_mutex_trylock(&mutex);
        assert(r == 0);
        r = __gthread_recursive_mutex_lock(&mutex);
        assert(r == 0);

        /* Add a resource.  */
        int old = resource;
        _MCF_sleep((const int64_t[]) { -10 });
        resource = old + 1;
        __gthread_recursive_mutex_unlock(&mutex);
        __gthread_recursive_mutex_unlock(&mutex);
        __gthread_recursive_mutex_unlock(&mutex);
        break;
      }
      else if(r == -1) {
        /* Wait.  */
        _MCF_sleep((const int64_t[]) { -10 });
        continue;
      }
      else
        assert(0);
    }

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return __MCF_nullptr;
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_create(&threads[k], thread_proc, __MCF_nullptr);
      assert(r == 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_join(threads[k], __MCF_nullptr);
      assert(r == 0);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
