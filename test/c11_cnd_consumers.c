/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];

#define NTICKS  100000
static mtx_t mutex;
static cnd_t cond_produced, cond_consumed;
static int value;
static int consumed[NTHREADS];

static
int
thread_proc(void* param)
  {
    int* my_consumed = param;

    for(;;) {
      /* Wait for value  */
      int err = mtx_lock(&mutex);
      assert(err == thrd_success);

      while(value == 0) {
        err = cnd_wait(&cond_produced, &mutex);
        assert(err == thrd_success);
        assert(mutex.__owner == _MCF_thread_self_tid());
        assert(mutex.__depth == 1);
      }

      /* Consume it  */
      int value_got = value;
      //printf("thread %d got %d\n", (int) _MCF_thread_self_tid(), value_got);
      if(value_got > 0)
        value = 0;

      err = cnd_signal(&cond_consumed);
      assert(err == thrd_success);

      err = mtx_unlock(&mutex);
      assert(err == thrd_success);

      if(value_got < 0)
        break;

      /* Accumulate it.  */
      *my_consumed += value_got;
    }

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    int err = mtx_init(&mutex, mtx_plain);
    assert(err == thrd_success);
    err = cnd_init(&cond_produced);
    assert(err == thrd_success);
    err = cnd_init(&cond_consumed);
    assert(err == thrd_success);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, &consumed[k]);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    _MCF_sleep((const int64_t[]) { -500 });

    err = mtx_lock(&mutex);
    assert(err == thrd_success);

    for(size_t k = 0;  k < NTICKS;  ++k) {
      /* Wait for consumption  */
      while(value != 0) {
        err = cnd_wait(&cond_consumed, &mutex);
        assert(err == thrd_success);
        assert(mutex.__owner == _MCF_thread_self_tid());
        assert(mutex.__depth == 1);
      }

      /* Produce one  */
      value = 1;
      //printf("main set %d\n", value);

      err = cnd_signal(&cond_produced);
      assert(err == thrd_success);
    }

    while(value != 0) {
      err = cnd_wait(&cond_consumed, &mutex);
      assert(err == thrd_success);
      assert(mutex.__owner == _MCF_thread_self_tid());
      assert(mutex.__depth == 1);
    }

    /* Inform end of input  */
    value = -1;
    printf("main set end of input\n");

    err = cnd_broadcast(&cond_produced);
    assert(err == thrd_success);

    err = mtx_unlock(&mutex);
    assert(err == thrd_success);

    /* Wait and sum all values  */
    int total = 0;

    printf("main waiting\n");
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], NULL);
      assert(r == thrd_success);
      printf("main wait finished: %d, consumed %d\n", (int)k, consumed[k]);
      total += consumed[k];
    }

    assert(total == NTICKS);
  }
