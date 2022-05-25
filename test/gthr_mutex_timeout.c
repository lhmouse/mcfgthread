/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include "../src/clock.h"
#include <assert.h>
#include <stdio.h>

static __gthread_mutex_t mutex = __GTHREAD_MUTEX_INIT;

int
main(void)
  {
    double now, delta;
    __gthread_time_t timeout;
    int r;

    /* Round the time up.  */
    int64_t sleep_until = (int64_t) time(NULL) * 1000 + 2000;
    _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(NULL) + 1;
    timeout.tv_nsec = 100000000;
    r = __gthread_mutex_timedlock(&mutex, &timeout);  /* lock it  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(NULL) + 1;
    timeout.tv_nsec = 100000000;
    r = __gthread_mutex_timedlock(&mutex, &timeout);
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1000);
    assert(delta <= 1200);
  }
