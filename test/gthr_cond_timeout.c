/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static __gthread_mutex_t mutex = __GTHREAD_MUTEX_INIT;
static __gthread_cond_t cond = __GTHREAD_COND_INIT;

int
main(void)
  {
    double now, delta;
    time_t sleep_until;
    __gthread_time_t timeout;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = __gthread_mutex_trylock(&mutex);
    assert(r == 0);

    sleep_until = time(__MCF_nullptr) + 2;
    _MCF_sleep(&(int64_t) { sleep_until * 1000LL - 20 });
    do { now = _MCF_perf_counter();
         timeout.tv_sec = time(__MCF_nullptr);
    } while(timeout.tv_sec < sleep_until);
    timeout.tv_sec += 1;
    timeout.tv_nsec = 115999999;  // relaxed
    r = __gthread_cond_timedwait(&cond, &mutex, &timeout);
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    r = __gthread_mutex_trylock(&mutex);
    assert(r == -1);
  }
