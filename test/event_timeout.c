/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_event event = _MCF_EVENT_INIT(42);

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_realtime);

    HMODULE winmm = LoadLibraryExW(L"WINMM.DLL", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(winmm) {
      typedef UINT __stdcall timeBeginPeriod_t(UINT);
      timeBeginPeriod_t* ptimeBeginPeriod = __MCF_CAST_PTR(timeBeginPeriod_t,
            GetProcAddress(winmm, "timeBeginPeriod"));
      if(ptimeBeginPeriod) {
        /* Increase timer resolution.  */
        ptimeBeginPeriod(1);
      }
    }

    r = _MCF_event_await_change(&event, -1, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);

    r = _MCF_event_await_change(&event, _MCF_EVENT_VALUE_MAX + 1, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 52, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 50);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, _MCF_EVENT_VALUE_MAX, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 50);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);

    now = _MCF_perf_counter();
    r = _MCF_event_await_change(&event, 42, &(int64_t){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);
  }
