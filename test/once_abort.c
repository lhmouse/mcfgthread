/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/once.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_once once;
static HANDLE event;
static int resource = 0;

static int num_init = 0;   /* threads that performed initialization  */
static int num_ready = 0;  /* threads that saw so but didn't do it  */

static
void
thread_proc(_MCF_thread* self)
  {
    WaitForSingleObject(event, INFINITE);

    int r = _MCF_once_wait(&once, NULL);
    printf("thread %d got %d\n", self->__tid, r);
    if(r == 1) {
      /* Perform initialization.  */
      int old = resource;
      Sleep(10);
      resource = old + 1;
      _MCF_once_abort(&once);

      Sleep(100);
      __atomic_fetch_add(&num_init, 1, __ATOMIC_RELAXED);
    }
    else if(r == 0) {
      /* Assume `resource` has been initialized.  */
      assert(resource == 1);

      Sleep(100);
      __atomic_fetch_add(&num_ready, 1, __ATOMIC_RELAXED);
    }
    else
      assert(0);

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
    assert(num_init == NTHREADS);
    assert(num_ready == 0);
  }
