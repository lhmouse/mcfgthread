/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/exit.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_thread* thr;

static
void
atexit_function(void)
  {
    _MCF_thread* cmp = _MCF_thread_self();
    assert(cmp);
    fprintf(stderr, "atexit thread = %p, tid = %d\n", cmp, cmp->__tid);

    assert(cmp == thr);
    assert(cmp->__tid == thr->__tid);
  }

int
main(void)
  {
    thr = _MCF_thread_self();
    assert(thr);
    fprintf(stderr, "main thread = %p, tid = %d\n", thr, thr->__tid);

    __MCF_atexit(atexit_function);
    fprintf(stderr, "main exiting\n");
    __MCF_exit(0);
  }
