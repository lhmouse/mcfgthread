/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/clock.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    int64_t timeout;
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

    now = _MCF_perf_counter();
    timeout = (int64_t) _MCF_hires_utc_now() + 1116;  /* absolute  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);

    now = _MCF_perf_counter();
    timeout = -1116;  /* relative  */
    r = _MCF_sleep(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);

    now = _MCF_perf_counter();
    timeout = (int64_t) _MCF_hires_utc_now() + 1116;  /* absolute  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);

    now = _MCF_perf_counter();
    timeout = -1116;  /* relative  */
    _MCF_sleep_noninterruptible(&timeout);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);
  }
