/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static __libcpp_mutex_t mutex = _LIBCPP_MUTEX_INITIALIZER;
static __libcpp_condvar_t cond = _LIBCPP_CONDVAR_INITIALIZER;

int
main(void)
  {
    double now, delta;
    time_t sleep_until;
    __libcpp_timespec_t timeout;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == true);

    sleep_until = time(__MCF_nullptr) + 2;
    _MCF_sleep(&(int64_t) { sleep_until * 1000LL - 20 });
    do { now = _MCF_perf_counter();
         timeout.tv_sec = time(__MCF_nullptr);
    } while(timeout.tv_sec < sleep_until);
    timeout.tv_sec += 1;
    timeout.tv_nsec = 115999999;  // relaxed
    r = __libcpp_condvar_timedwait(&cond, &mutex, &timeout);
    assert(r == ETIMEDOUT);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == false);
  }
