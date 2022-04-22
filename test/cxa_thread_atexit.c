/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxa.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>

static
void
thread_atexit_proc(void* arg)
  {
    printf("thread %d atexit\n", (int) _MCF_thread_self_tid());
    *(int*) arg = 42;
  }

static
void
thread_proc(_MCF_thread* self)
  {
    __MCF_cxa_thread_atexit(thread_atexit_proc, self->__data, NULL);
    int64_t sleep_time = -1000;
    _MCF_sleep(&sleep_time);
    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, NULL, sizeof(int));
    assert(thrd);

    int* value = (int*) thrd->__data;
    assert(*value == 0);

    printf("main waiting\n");
    _MCF_thread_wait(thrd, NULL);
    printf("main wait finished\n");

    assert(*value == 42);
  }
