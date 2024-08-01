/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];
static once_flag once = ONCE_FLAG_INIT;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
void
once_do_it(void)
  {
    /* Perform initialization.  */
    int old = resource;
    _MCF_sleep((const int64_t[]) { -200 });
    resource = old + 1;

    _MCF_sleep((const int64_t[]) { -100 });
  }

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, __MCF_nullptr);

    call_once(&once, once_do_it);
    printf("thread %d once\n", (int) _MCF_thread_self_tid());

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, __MCF_nullptr);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], __MCF_nullptr);
      assert(r == thrd_success);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == 1);
  }
