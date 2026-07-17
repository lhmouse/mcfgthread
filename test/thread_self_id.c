/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];

static
void
thread_proc(_MCF_thread* self)
  {
    assert(_MCF_thread_self_tid() == GetCurrentThreadId());
    assert(_MCF_thread_get_tid(self) == GetCurrentThreadId());
    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], NULL);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }
  }
