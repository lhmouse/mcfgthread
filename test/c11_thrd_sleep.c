/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    struct timespec timeout, rem;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    now = _MCF_perf_counter();
    timeout.tv_sec = 1;  /* relative  */
    timeout.tv_nsec = 315999999;  // relaxed
    rem.tv_sec = 5;
    rem.tv_nsec = 5;
    r = thrd_sleep(&timeout, &rem);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1300);
    assert(delta <= 1400);
    assert(rem.tv_sec == 0);
    assert(rem.tv_nsec == 0);

    now = _MCF_perf_counter();
    timeout.tv_sec = 0;
    timeout.tv_nsec = 0;
    rem.tv_sec = 5;
    rem.tv_nsec = 5;
    r = thrd_sleep(&timeout, &rem);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);
    assert(rem.tv_sec == 0);
    assert(rem.tv_nsec == 0);
  }
