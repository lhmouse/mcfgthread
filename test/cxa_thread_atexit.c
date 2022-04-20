/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxa.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static
void
thread_atexit_proc(void* arg)
  {
    printf("thread %d atexit\n", (int) GetCurrentThreadId());
    *(int*) arg = 42;
  }

static
void
thread_proc(_MCF_thread* self)
  {
    __MCF_cxa_thread_atexit(thread_atexit_proc, self->__data, NULL);
    Sleep(1000);
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
    WaitForSingleObject(thrd->__handle, INFINITE);
    printf("main wait finished\n");

    assert(*value == 42);
  }
