/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];

static
void
thread_proc(_MCF_thread* self)
  {
    assert(_MCF_thread_self_tid() == GetCurrentThreadId());
    assert(self->__tid == GetCurrentThreadId());
    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], NULL);
      printf("main wait finished: %d\n", (int)k);
    }
  }
