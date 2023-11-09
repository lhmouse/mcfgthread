/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

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

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == true);

    /* Round the time up.  */
    int64_t sleep_until = (int64_t) time(NULL) * 1000 + 2000;
    _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(NULL) + 1;
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
