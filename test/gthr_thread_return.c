/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static __gthread_t thrd;
static int magic;

static
void*
thread_proc(void* param)
  {
    (void) param;
    Sleep(500);

    printf("thread %d quitting\n", (int) GetCurrentThreadId());
    return &magic;
  }

int
main(void)
  {
    int r = __gthread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    printf("main waiting\n");
    void* ret;
    r = __gthread_join(thrd, &ret);
    assert(r == 0);
    printf("main wait finished: %p\n", ret);

    assert(ret == &magic);
  }
