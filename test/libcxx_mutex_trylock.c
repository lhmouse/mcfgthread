/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __libcpp_thread_t threads[NTHREADS];
static __libcpp_mutex_t mutex = _LIBCPP_MUTEX_INITIALIZER;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    for(;;) {
      int r = __libcpp_mutex_trylock(&mutex);
      if(r == true) {
        printf("thread %d got %d\n", (int) _MCF_thread_self_tid(), r);

        /* Add a resource.  */
        int old = resource;
        _MCF_sleep((const int64_t[]) { -10 });
        resource = old + 1;
        __libcpp_mutex_unlock(&mutex);
        break;
      }
      else if(r == false) {
        /* Wait.  */
        _MCF_sleep((const int64_t[]) { -10 });
        continue;
      }
      else
        assert(0);
    }

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
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
