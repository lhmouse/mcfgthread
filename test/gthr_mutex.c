// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static __gthread_mutex_t mutex = __GTHREAD_MUTEX_INIT;
static HANDLE event;
static int resource = 0;

static void*
thread_proc(void* param)
  {
    (void) param;
    WaitForSingleObject(event, INFINITE);

    int r = __gthread_mutex_lock(&mutex);
    assert(r == 0);

    // Add a resource.
    int old = resource;
    Sleep(10);
    resource = old + 1;
    __gthread_mutex_unlock(&mutex);

    printf("thread %d quitting\n", (int) GetCurrentThreadId());
    return NULL;
  }

int
main(void)
  {
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(event);

#define NTHREADS  64U
    __gthread_t threads[NTHREADS];
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_create(&threads[k], thread_proc, NULL);
      assert(r == 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    SetEvent(event);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = __gthread_join(threads[k], NULL);
      assert(r == 0);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
