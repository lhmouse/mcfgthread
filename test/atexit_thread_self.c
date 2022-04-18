/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxa.h"
#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>

static _MCF_thread* thr;

static void
atexit_function(void)
  {
    _MCF_thread* cmp = _MCF_thread_self();
    assert(cmp);
    printf("atexit thread = %p, tid = %d\n", (void*) cmp, (int) cmp->__tid);

    assert(cmp == thr);
    assert(cmp->__tid == thr->__tid);
  }

int
main(void)
  {
    thr = _MCF_thread_self();
    assert(thr);
    printf("main thread = %p, tid = %d\n", (void*) thr, (int) thr->__tid);

    __MCF_atexit(atexit_function);
    printf("main exiting\n");
  }
