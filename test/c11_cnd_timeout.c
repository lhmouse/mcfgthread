/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static mtx_t mutex;
static cnd_t cond;

int
main(void)
  {
    double now, delta;
    time_t sleep_until;
    struct timespec timeout;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = mtx_init(&mutex, mtx_timed);
    assert(r == thrd_success);
    r = cnd_init(&cond);
    assert(r == thrd_success);
    r = mtx_trylock(&mutex);
    assert(r == thrd_success);

    sleep_until = time(__MCF_nullptr) + 2;
    _MCF_sleep(&(int64_t) { sleep_until * 1000LL - 20 });
    do { now = _MCF_perf_counter();
         timeout.tv_sec = time(__MCF_nullptr);
    } while(timeout.tv_sec < sleep_until);
    timeout.tv_sec += 1;
    timeout.tv_nsec = 115999999;  // relaxed
    r = cnd_timedwait(&cond, &mutex, &timeout);
    assert(r == thrd_timedout);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    r = mtx_trylock(&mutex);
    assert(r == thrd_error);
  }
