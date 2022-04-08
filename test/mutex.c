// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/mutex.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_mutex mutex;
static HANDLE event;
static int resource = 0;

static void
thread_proc(_MCF_thread* self)
  {
    WaitForSingleObject(event, INFINITE);

    int r = _MCF_mutex_lock(&mutex, NULL);
    assert(r == 0);

    // Add a resource.
    int old = resource;
    Sleep(10);
    resource = old + 1;
    _MCF_mutex_unlock(&mutex);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(event);

#define NTHREADS  64U
    _MCF_thread* threads[NTHREADS];
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    SetEvent(event);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      WaitForSingleObject(threads[k]->__handle, INFINITE);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
