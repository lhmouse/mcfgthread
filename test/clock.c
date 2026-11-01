/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#define _POSIX_C_SOURCE 200809
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

/* Get the current UTC time in milliseconds since the Unix epoch. Do not ask
 * for the local time and convert it back with `mktime()`: that makes the
 * result depend on the C runtime's idea of the time zone, which may disagree
 * with the operating system's by an hour around daylight saving time
 * transitions. There is no need to call `_tzset()` either.  */
static
long long
do_utc_now_msec(void)
  {
    SYSTEMTIME st;
    GetSystemTime(&st);  /* This is UTC.  */
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    return (long long) (ull.QuadPart / 10000) - 11644473600000LL;
  }

int
main(void)
  {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    long long utc_now = do_utc_now_msec();
    fprintf(stderr, "utc_now = %lld\n", utc_now);

    long long ms_now = _MCF_utc_now();
    long long ms_delta = ms_now - utc_now;
    fprintf(stderr, "_MCF_utc_now() = %lld (delta %+lld)\n", ms_now, ms_delta);
    assert(ms_delta >= -100);
    assert(ms_delta <= +100);

    double hi_now = _MCF_hires_utc_now();
    double hi_delta = hi_now - (double) utc_now;
    fprintf(stderr, "_MCF_hires_utc_now() = %.0f (delta %+.3f)\n", hi_now, hi_delta);
    assert(hi_delta >= -100);
    assert(hi_delta <= +100);

    /* The C runtime's UTC clock agrees as well, to within a second.  */
    long long vcrt_sec = (long long) time(0);
    long long sec_delta = ms_now / 1000 - vcrt_sec;
    fprintf(stderr, "time() = %lld (delta %+lld s)\n", vcrt_sec, sec_delta);
    assert(sec_delta >= -1);
    assert(sec_delta <= +1);
  }
