/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/xglobals.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    __MCF_winnt_timeout to;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);
    now = _MCF_perf_counter();

    __MCF_initialize_winnt_timeout_v3(&to, (const int64_t[]){ -1100 });  /* relative  */
    while(to.__li.QuadPart < 0) {
      // repeat
      printf("  sleep -> %lld\n", to.__li.QuadPart);
      _MCF_sleep_noninterruptible((const int64_t[]){ -37 });
      __MCF_adjust_winnt_timeout_v3(&to);
    }

    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);
  }
