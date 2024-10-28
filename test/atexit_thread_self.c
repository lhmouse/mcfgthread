/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>

static _MCF_thread* thr;

static
void
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
