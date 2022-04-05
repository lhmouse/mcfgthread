// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/mutex.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_mutex mutex;
static HANDLE event;
static int resource = 0;

static DWORD __stdcall
thread_proc(void* param)
  {
    (void)param;
    WaitForSingleObject(event, INFINITE);
    const int myid = (int) GetCurrentThreadId();

    for(;;) {
      const int64_t timeout = 0;
      int r = _MCF_mutex_lock(&mutex, &timeout);
      if(r == 0) {
        printf("thread %d got %d\n", myid, r);

        // Add a resource.
        int old = resource;
        Sleep(10);
        resource = old + 1;
        _MCF_mutex_unlock(&mutex);
        break;
      }
      else if(r == -1) {
        // Wait.
        Sleep(10);
        continue;
      }
      else
        assert(0);
    }

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
  }
