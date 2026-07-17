/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_event event = __MCF_EVENT_INIT(42);

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    r = _MCF_event_await_change(&event, -1, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);

    r = _MCF_event_await_change(&event, __MCF_EVENT_VALUE_MAX + 1, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 52, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, __MCF_EVENT_VALUE_MAX, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, &(int64_t){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);
  }
