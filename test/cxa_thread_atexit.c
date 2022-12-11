/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/thread.h"
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
    __MCF_cxa_thread_atexit(thread_atexit_proc, _MCF_thread_get_data(self), NULL);
    _MCF_sleep((const int64_t[]) { -1000 });
    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, NULL, sizeof(int));
    assert(thrd);

    int* value = _MCF_thread_get_data(thrd);
    assert(*value == 0);

    printf("main waiting\n");
    _MCF_thread_wait(thrd, NULL);
    printf("main wait finished\n");

    assert(*value == 42);
  }
