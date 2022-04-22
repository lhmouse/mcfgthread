/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __gthread_t threads[NTHREADS];
static __gthread_mutex_t mutex = __GTHREAD_MUTEX_INIT;
static _MCF_once start;
static int resource = 0;

static
void*
thread_proc(void* param)
  {
    (void) param;
    _MCF_once_wait(&start, NULL);
    _MCF_once_wait(&start, NULL);

    for(;;) {
      int r = __gthread_mutex_trylock(&mutex);
      if(r == 0) {
        printf("thread %d got %d\n", (int) _MCF_thread_self_tid(), r);

        /* Add a resource.  */
        int old = resource;
        int64_t sleep_time = -10;
        _MCF_sleep(&sleep_time);
        resource = old + 1;
        __gthread_mutex_unlock(&mutex);
        break;
      }
      else if(r == EBUSY) {
        /* Wait.  */
        int64_t sleep_time = -10;
        _MCF_sleep(&sleep_time);
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
      int r = __gthread_create(&threads[k], thread_proc, NULL);
      assert(r == 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_once_release(&start);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_join(threads[k], NULL);
      assert(r == 0);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
