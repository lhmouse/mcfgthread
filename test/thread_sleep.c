/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
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
    timeout = (int64_t) _MCF_hires_utc_now() + 1116;  /* absolute  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = -1116;  /* relative  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = (int64_t) _MCF_hires_utc_now() + 1116;  /* absolute  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    timeout = -1116;  /* relative  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);
  }
