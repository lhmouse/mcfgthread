/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
#include "../src/clock.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    struct timespec timeout, rem;
    int r;

    /* Round the time up.  */
    int64_t sleep_until = (int64_t) time(NULL) * 1000 + 2000;
    _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = 1;  /* relative  */
    timeout.tv_nsec = 300000000;
    rem.tv_sec = 5;
    rem.tv_nsec = 5;
    r = thrd_sleep(&timeout, &rem);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1300 - 20);
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
    printf("delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);
    assert(rem.tv_sec == 0);
    assert(rem.tv_nsec == 0);
  }
