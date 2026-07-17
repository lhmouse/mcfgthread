/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    struct timespec timeout, rem;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

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
