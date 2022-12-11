/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static _MCF_event event = __MCF_EVENT_INIT(42);

int
main(void)
  {
    double now, delta;
    int r;

    r = _MCF_event_await_change(&event, -1, (const int64_t[]){ _MCF_utc_now() + 1100 });  /* absolute  */
    assert(r == -2);

    r = _MCF_event_await_change(&event, __MCF_EVENT_VALUE_MAX + 1, (const int64_t[]){ _MCF_utc_now() + 1100 });  /* absolute  */
    assert(r == -2);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 52, (const int64_t[]){ _MCF_utc_now() + 1100 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, __MCF_EVENT_VALUE_MAX, (const int64_t[]){ _MCF_utc_now() + 1100 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, (const int64_t[]){ _MCF_utc_now() + 1100 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1000);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, (const int64_t[]){ -1100 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1000);
    assert(delta <= 1200);
  }
