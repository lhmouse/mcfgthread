// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static __gthread_once_t once = __GTHREAD_ONCE_INIT;
static HANDLE event;
static int resource = 0;

static void
once_do_it(void)
  {
    // Perform initialization.
    int old = resource;
    Sleep(200);
    resource = old + 1;

    Sleep(100);
  }

static void*
thread_proc(void* param)
  {
    (void) param;
    WaitForSingleObject(event, INFINITE);

    int err = __gthread_once(&once, once_do_it);
    printf("thread %d got %d\n", (int) GetCurrentThreadId(), err);
    assert(err == 0);

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

    assert(resource == 1);
  }
