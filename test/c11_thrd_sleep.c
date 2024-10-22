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

    /* Round the time up. The `while` loop is necessary to work around a bug
     * in Wine, which physical Windows doesn't have.
     * See https://bugs.winehq.org/show_bug.cgi?id=57035  */
    int64_t sleep_until = (int64_t) time(__MCF_nullptr) * 1000 + 2000;
    while(time(__MCF_nullptr) < sleep_until / 1000)
      _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = 1;  /* relative  */
    timeout.tv_nsec = 300000000;
    rem.tv_sec = 5;
    rem.tv_nsec = 5;
    r = thrd_sleep(&timeout, &rem);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1300 - 40);
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
