/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    /* Round the time up.  */
    int64_t sleep_until = (int64_t) time(__MCF_nullptr) * 1000 + 2000;
    _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    __MCF_libcxx_thread_sleep_for_ns_count(1300000000);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1300 - 40);
    assert(delta <= 1400);
  }
