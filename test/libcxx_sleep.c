/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    /* Round the time up. The `while` loop is necessary to work around a bug
     * in Wine, which physical Windows doesn't have.
     * See https://bugs.winehq.org/show_bug.cgi?id=57035  */
    int64_t sleep_until = (int64_t) time(__MCF_nullptr) * 1000 + 2000;
    while(time(__MCF_nullptr) < sleep_until / 1000)
      _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    __MCF_libcxx_thread_sleep_for_ns_count(1300000000);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1300 - 40);
    assert(delta <= 1400);
  }
