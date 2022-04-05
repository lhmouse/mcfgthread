// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/once.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_once once;
static HANDLE event;
static int resource = 0;

static int num_init = 0;   // threads that performed initialization
static int num_ready = 0;  // threads that saw so but didn't do it

static DWORD __stdcall
thread_proc(void* param)
  {
    (void)param;
    WaitForSingleObject(event, INFINITE);
    const int myid = (int) GetCurrentThreadId();

    int r = _MCF_once_wait(&once, NULL);
    printf("thread %d got %d\n", myid, r);
    if(r == 1) {
      // Perform initialization.
      int old = resource;
      Sleep(10);
      resource = old + 1;
      _MCF_once_abort(&once);

      Sleep(100);
      __atomic_fetch_add(&num_init, 1, __ATOMIC_RELAXED);
    }
    else if(r == 0) {
      // Assume `resource` has been initialized.
      assert(resource == 1);

      Sleep(100);
      __atomic_fetch_add(&num_ready, 1, __ATOMIC_RELAXED);
    }
    else
      assert(0);

    printf("thread %d quitting\n", myid);
    return 0;
  }

int
main(void)
  {
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(event);

#define NTHREADS  64
    HANDLE threads[NTHREADS];
    for(size_t k = 0;  k < NTHREADS;  ++k)
      threads[k] = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);

    printf("main waiting\n");
    SetEvent(event);
    DWORD wait = WaitForMultipleObjects(NTHREADS, threads, TRUE, INFINITE);
    printf("main wait finished: %d\n", (int)wait);
    assert(wait < WAIT_OBJECT_0 + NTHREADS);

    assert(resource == NTHREADS);
    assert(num_init == NTHREADS);
    assert(num_ready == 0);
  }
