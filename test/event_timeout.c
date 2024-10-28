/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static _MCF_event event = __MCF_EVENT_INIT(42);

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = _MCF_event_await_change(&event, -1, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -2);

    r = _MCF_event_await_change(&event, __MCF_EVENT_VALUE_MAX + 1, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -2);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 52, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, __MCF_EVENT_VALUE_MAX, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, (const int64_t[]){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);
  }
