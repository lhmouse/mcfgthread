/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static mtx_t mutex;

int
main(void)
  {
    double now, delta;
    time_t sleep_until;
    struct timespec timeout;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    r = mtx_init(&mutex, mtx_recursive | mtx_timed);
    assert(r == thrd_success);

    sleep_until = time(NULL) + 2;
    _MCF_sleep_noninterruptible(&(int64_t){ sleep_until * 1000LL - 20 });
    do { now = _MCF_perf_counter();
         timeout.tv_sec = time(NULL);
    } while(timeout.tv_sec < sleep_until);
    timeout.tv_sec += 1;
    timeout.tv_nsec = 115999999;  // relaxed
    r = mtx_timedlock(&mutex, &timeout);  /* lock it  */
    assert(r == thrd_success);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    sleep_until = time(NULL) + 2;
    _MCF_sleep_noninterruptible(&(int64_t){ sleep_until * 1000LL - 20 });
    do { now = _MCF_perf_counter();
         timeout.tv_sec = time(NULL);
    } while(timeout.tv_sec < sleep_until);
    timeout.tv_sec += 1;
    timeout.tv_nsec = 115999999;  // relaxed
    r = mtx_timedlock(&mutex, &timeout);
    assert(r == thrd_success);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);
  }
