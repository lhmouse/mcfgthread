/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include "../mcfgthread/src/xglobals.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    double now, delta;
    __MCF_winnt_timeout to;

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

    __MCF_initialize_winnt_timeout_v3(&to, &(int64_t){ -1116 });  /* relative  */
    while(to.li.QuadPart < 0) {
      // repeat
      fprintf(stderr, "  sleep -> %lld\n", to.li.QuadPart);
      _MCF_sleep_noninterruptible(&(int64_t){ -37 });
      __MCF_adjust_winnt_timeout_v3(&to);
    }

    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1111);
    assert(delta <= 1166);
  }
