// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  64U
static __gthread_t threads[NTHREADS];

#define NTICKS  1000000
static __gthread_recursive_mutex_t mutex;
static __gthread_cond_t cond_produced, cond_consumed;
static int value;
static int consumed[NTHREADS];

static void*
thread_proc(void* param)
  {
    int* my_consumed = param;

    for(;;) {
      // Wait for value
      int err = __gthread_recursive_mutex_lock(&mutex);
      assert(err == 0);
      err = __gthread_recursive_mutex_lock(&mutex);
      assert(err == 0);
      err = __gthread_recursive_mutex_lock(&mutex);
      assert(err == 0);

      while(value == 0) {
        err = __gthread_cond_wait_recursive(&cond_produced, &mutex);
        assert(err == 0);
        assert(mutex.__mutex.__locked);
        assert(mutex.__depth == 3);
        assert(mutex.__owner == GetCurrentThreadId());
      }

      // Consume it
      int value_got = value;
      //printf("thread %d got %d\n", (int) GetCurrentThreadId(), value_got);
      if(value_got > 0)
        value = 0;

      err = __gthread_cond_signal(&cond_consumed);
      assert(err == 0);

      err = __gthread_recursive_mutex_unlock(&mutex);
      assert(err == 0);
      err = __gthread_recursive_mutex_unlock(&mutex);
      assert(err == 0);
      err = __gthread_recursive_mutex_unlock(&mutex);
      assert(err == 0);

      if(value_got < 0)
        break;

      // Accumulate it.
      *my_consumed += value_got;
    }

    printf("thread %d quitting\n", (int) GetCurrentThreadId());
    return NULL;
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_create(&threads[k], thread_proc, &consumed[k]);
      assert(r == 0);
      assert(threads[k]);
    }

    Sleep(500);

    int err = __gthread_recursive_mutex_lock(&mutex);
    assert(err == 0);
    err = __gthread_recursive_mutex_lock(&mutex);
    assert(err == 0);
    err = __gthread_recursive_mutex_lock(&mutex);
    assert(err == 0);

    for(size_t k = 0;  k < NTICKS;  ++k) {
      // Wait for consumption
      while(value != 0) {
        err = __gthread_cond_wait_recursive(&cond_consumed, &mutex);
        assert(err == 0);
        assert(mutex.__mutex.__locked);
        assert(mutex.__depth == 3);
        assert(mutex.__owner == GetCurrentThreadId());
      }

      // Produce one
      value = 1;
      //printf("main set %d\n", value);

      err = __gthread_cond_signal(&cond_produced);
      assert(err == 0);
    }

    while(value != 0) {
      err = __gthread_cond_wait_recursive(&cond_consumed, &mutex);
      assert(err == 0);
      assert(mutex.__mutex.__locked);
      assert(mutex.__depth == 3);
      assert(mutex.__owner == GetCurrentThreadId());
    }

    // Inform end of input
    value = -1;
    printf("main set end of input\n");

    err = __gthread_cond_broadcast(&cond_produced);
    assert(err == 0);

    err = __gthread_recursive_mutex_unlock(&mutex);
    assert(err == 0);
    err = __gthread_recursive_mutex_unlock(&mutex);
    assert(err == 0);
    err = __gthread_recursive_mutex_unlock(&mutex);
    assert(err == 0);

    // Wait and sum all values
    int total = 0;

    printf("main waiting\n");
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_join(threads[k], NULL);
      assert(r == 0);
      printf("main wait finished: %d, consumed %d\n", (int)k, consumed[k]);
      total += consumed[k];
    }

    assert(total == NTICKS);
  }
