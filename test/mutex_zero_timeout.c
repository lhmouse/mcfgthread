/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/mutex.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_mutex mutex;
static HANDLE event;
static int resource = 0;

static
void
thread_proc(_MCF_thread* self)
  {
    WaitForSingleObject(event, INFINITE);

    for(;;) {
      const int64_t timeout = 0;
      int r = _MCF_mutex_lock(&mutex, &timeout);
      if(r == 0) {
        printf("thread %d got %d\n", self->__tid, r);

        /* Add a resource.  */
        int old = resource;
        Sleep(10);
        resource = old + 1;
        _MCF_mutex_unlock(&mutex);
        break;
      }
      else if(r == -1) {
        /* Wait.  */
        Sleep(10);
        continue;
      }
      else
        assert(0);
    }

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(event);

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
