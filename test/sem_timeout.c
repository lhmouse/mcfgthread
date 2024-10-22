/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/sem.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = __MCF_SEM_INIT(1);

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    now = _MCF_perf_counter();
    r = _MCF_sem_wait(&sem, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1100 });  /* absolute  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_sem_wait(&sem, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1100 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_sem_wait(&sem, (const int64_t[]){ -1100 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);
  }
