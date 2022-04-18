/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  64U
static __gthread_t threads[NTHREADS];
static __gthread_key_t key;
static HANDLE event;
static int count;

static void
tls_destructor(void* ptr)
  {
    printf("thread %d tls_destructor\n", (int) GetCurrentThreadId());
    __atomic_fetch_add((int*) ptr, 1, __ATOMIC_RELAXED);
  }

static void*
thread_proc(void* param)
  {
    (void) param;
    WaitForSingleObject(event, INFINITE);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    printf("thread %d quitting\n", (int) GetCurrentThreadId());
    return NULL;
  }

int
main(void)
  {
    int r = __gthread_key_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(event);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = __gthread_create(&threads[k], thread_proc, NULL);
      assert(r == 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    SetEvent(event);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      r = __gthread_join(threads[k], NULL);
      assert(r == 0);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
