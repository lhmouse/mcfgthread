/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/c11.h"
#include <assert.h>
#include <stdio.h>

static thrd_t thrd;
static int magic = 123456789;

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sleep((const int64_t[]) { -500 });

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return magic;
  }

int
main(void)
  {
    int r = thrd_create(&thrd, thread_proc, NULL);
    assert(r == thrd_success);
    assert(thrd);

    printf("main waiting\n");
    int ret = 6543210;
    r = thrd_join(thrd, &ret);
    assert(r == thrd_success);
    printf("main wait finished: %d\n", ret);

    assert(ret == magic);
  }
