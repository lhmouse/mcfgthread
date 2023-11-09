/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    int64_t timeout;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    now = _MCF_perf_counter();
    timeout = (int64_t) _MCF_hires_utc_now() + 1100;  /* absolute  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = -1100;  /* relative  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = (int64_t) _MCF_hires_utc_now() + 1100;  /* absolute  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = -1100;  /* relative  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);
  }
