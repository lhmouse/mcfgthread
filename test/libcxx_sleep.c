/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta, max, min, total;

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

    max = 12345;
    min = 12345;
    total = 0;
    for(int k = 0;  k < 10;  ++k) {
      now = _MCF_perf_counter();
      __MCF_libcxx_thread_sleep_for_ns_count(1300000000);
      delta = _MCF_perf_counter() - now;
      total += delta;
      if((max == 12345) || (delta > max))
        max = delta;
      if((min == 12345) || (delta < min))
        min = delta;
    }
    delta = (total - max - min) / 8;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1295);
    assert(delta <= 1350);
  }
