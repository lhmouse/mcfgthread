/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
#if defined(__i386__) || defined(__amd64__)
    assert(sizeof(long double) > 8);
    static volatile long double eps = 0x0.000000000000001p0L;
    static volatile long double ep1 = 0x1.000000000000001p0L;

    assert(ep1 != 1);
    assert(eps + 1 == ep1);
#endif  // x87

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thrd);

    printf("main waiting\n");
    _MCF_thread_wait(thrd, NULL);
    printf("main wait finished\n");

    _MCF_thread_wait(thrd, NULL);
  }
