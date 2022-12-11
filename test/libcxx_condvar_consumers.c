/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static __libcpp_thread_t threads[NTHREADS];

#define NTICKS  100000
static __libcpp_mutex_t mutex;
static __libcpp_condvar_t cond_produced, cond_consumed;
static int value;
static int consumed[NTHREADS];

static
void*
thread_proc(void* param)
  {
    int* my_consumed = param;

    for(;;) {
      /* Wait for value  */
      int err = __libcpp_mutex_lock(&mutex);
      assert(err == 0);

      while(value == 0) {
        err = __libcpp_condvar_wait(&cond_produced, &mutex);
        assert(err == 0);
        assert(mutex.__locked);
      }

      /* Consume it  */
      int value_got = value;
      //printf("thread %d got %d\n", (int) _MCF_thread_self_tid(), value_got);
      if(value_got > 0)
        value = 0;

      err = __libcpp_condvar_signal(&cond_consumed);
      assert(err == 0);

      err = __libcpp_mutex_unlock(&mutex);
      assert(err == 0);

      if(value_got < 0)
        break;

      /* Accumulate it.  */
      *my_consumed += value_got;
    }

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return NULL;
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __libcpp_thread_create(&threads[k], thread_proc, &consumed[k]);
      assert(r == 0);
      assert(threads[k]);
    }

    _MCF_sleep((const int64_t[]) { -500 });

    int err = __libcpp_mutex_lock(&mutex);
    assert(err == 0);

    for(size_t k = 0;  k < NTICKS;  ++k) {
      /* Wait for consumption  */
      while(value != 0) {
        err = __libcpp_condvar_wait(&cond_consumed, &mutex);
        assert(err == 0);
        assert(mutex.__locked);
      }

      /* Produce one  */
      value = 1;
      //printf("main set %d\n", value);

      err = __libcpp_condvar_signal(&cond_produced);
      assert(err == 0);
    }

    while(value != 0) {
      err = __libcpp_condvar_wait(&cond_consumed, &mutex);
      assert(err == 0);
      assert(mutex.__locked);
    }

    /* Inform end of input  */
    value = -1;
    printf("main set end of input\n");

    err = __libcpp_condvar_broadcast(&cond_produced);
    assert(err == 0);

    err = __libcpp_mutex_unlock(&mutex);
    assert(err == 0);

    /* Wait and sum all values  */
    int total = 0;

    printf("main waiting\n");
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __libcpp_thread_join(&threads[k]);
      assert(r == 0);
      printf("main wait finished: %d, consumed %d\n", (int)k, consumed[k]);
      total += consumed[k];
    }

    assert(total == NTICKS);
  }
