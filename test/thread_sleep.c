/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include "../src/clock.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    int64_t timeout;
    int r;

    now = _MCF_perf_counter();
    timeout = _MCF_utc_now() + 1100;  /* absolute  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 20);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = -1100;  /* relative  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 20);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = _MCF_utc_now() + 1100;  /* absolute  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 20);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = -1100;  /* relative  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 20);
    assert(delta <= 1200);
  }
