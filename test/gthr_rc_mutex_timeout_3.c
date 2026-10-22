/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __gthread_recursive_mutex_t mutex = __GTHREAD_RECURSIVE_MUTEX_INIT;

int
main(void)
  {
    double now, delta, max, min, total;
    time_t sleep_until;
    __gthread_time_t timeout;
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

    r = __gthread_recursive_mutex_trylock(&mutex);
    assert(r == 0);

    max = 12345;
    min = 12345;
    total = 0;
    for(int k = 0;  k < 10;  ++k) {
      if(k > 0)
        __gthread_recursive_mutex_unlock(&mutex);
      sleep_until = time(NULL) + 1;
      _MCF_sleep_noninterruptible(&(int64_t){ sleep_until * 1000LL - 20 });
      do {
        _MCF_yield();
        now = _MCF_perf_counter();
        timeout.tv_sec = time(NULL);
      } while(timeout.tv_sec < sleep_until);
      timeout.tv_sec += 1;
      timeout.tv_nsec = 115999999;  // relaxed
      r = __gthread_recursive_mutex_timedlock(&mutex, &timeout);
      assert(r == 0);
      delta = _MCF_perf_counter() - now;
      total += delta;
      if((max == 12345) || (delta > max))
        max = delta;
      if((min == 12345) || (delta < min))
        min = delta;
    }
    delta = (total - max - min) / 8;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 50);
  }
