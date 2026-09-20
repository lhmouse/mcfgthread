/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    struct timespec timeout, rem;
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
    timeout.tv_sec = 1;  /* relative  */
    timeout.tv_nsec = 315999999;  // relaxed
    rem.tv_sec = 5;
    rem.tv_nsec = 5;
    r = thrd_sleep(&timeout, &rem);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1311);
    assert(delta <= 1366);
    assert(rem.tv_sec == 0);
    assert(rem.tv_nsec == 0);

    now = _MCF_perf_counter();
    timeout.tv_sec = 0;
    timeout.tv_nsec = 0;
    rem.tv_sec = 5;
    rem.tv_nsec = 5;
    r = thrd_sleep(&timeout, &rem);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 50);
    assert(rem.tv_sec == 0);
    assert(rem.tv_nsec == 0);
  }
