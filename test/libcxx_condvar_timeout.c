/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
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
    __libcpp_timespec_t timeout;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == true);

    /* Round the time up. The `while` loops is necessary to work around a bug
     * in Wine, which physical Windows doesn't have.
     * See https://bugs.winehq.org/show_bug.cgi?id=57035  */
    int64_t sleep_until = (int64_t) time(__MCF_nullptr) * 1000 + 2000;
    while(time(__MCF_nullptr) * 1000 < sleep_until)
      _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(__MCF_nullptr) + 1;
    timeout.tv_nsec = 100000000;
    r = __libcpp_condvar_timedwait(&cond, &mutex, &timeout);
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == false);
  }
