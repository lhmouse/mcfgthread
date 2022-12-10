/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
#include "../src/clock.h"
#include <assert.h>
#include <stdio.h>

static mtx_t mutex;
static cnd_t cond;

int
main(void)
  {
    double now, delta;
    struct timespec timeout;
    int r;

    r = mtx_init(&mutex, mtx_timed);
    assert(r == thrd_success);
    r = cnd_init(&cond);
    assert(r == thrd_success);
    r = mtx_trylock(&mutex);
    assert(r == thrd_success);

    /* Round the time up.  */
    int64_t sleep_until = (int64_t) time(NULL) * 1000 + 2000;
    _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(NULL) + 1;
    timeout.tv_nsec = 100000000;
    r = cnd_timedwait(&cond, &mutex, &timeout);
    assert(r == thrd_timedout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 20);
    assert(delta <= 1200);

    r = mtx_trylock(&mutex);
    assert(r == thrd_error);
  }
