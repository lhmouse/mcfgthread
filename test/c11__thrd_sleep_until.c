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
    time_t sleep_until;
    struct timespec timeout;
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

    sleep_until = time(NULL) + 2;
    _MCF_sleep_noninterruptible(&(int64_t){ sleep_until * 1000LL - 20 });
    do { now = _MCF_perf_counter();
         timeout.tv_sec = time(NULL);
    } while(timeout.tv_sec < sleep_until);
    timeout.tv_sec += 1;
    timeout.tv_nsec = 315999999;  // relaxed
    r = _thrd_sleep_until(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1311);
    assert(delta <= 1366);

    now = _MCF_perf_counter();
    timeout.tv_sec = 0;
    timeout.tv_nsec = 0;
    r = _thrd_sleep_until(&timeout);
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 50);
  }
