/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#ifdef _USE_32BIT_TIME_T
#  define my_mktime   _mktime64
#else
#  define my_mktime   mktime
#endif

int
main(void)
  {
    tzset();

    SYSTEMTIME st;
    GetLocalTime(&st);

    struct tm tm;
    tm.tm_year = st.wYear - 1900;
    tm.tm_mon = st.wMonth - 1;
    tm.tm_mday = st.wDay;
    tm.tm_hour = st.wHour;
    tm.tm_min = st.wMinute;
    tm.tm_sec = st.wSecond;
    int64_t vcrt_now = my_mktime(&tm) * 1000 + st.wMilliseconds;
    printf("vcrt_now = %lld\n", vcrt_now);

    int64_t ms_now = _MCF_utc_now();
    int64_t ms_delta = ms_now - vcrt_now;
    printf("_MCF_utc_now() = %lld (delta %+lld)\n", ms_now, ms_delta);
    assert(ms_delta >= -100);
    assert(ms_delta <= +100);

    double hi_now = _MCF_hires_utc_now();
    double hi_delta = hi_now - (double) vcrt_now;
    printf("_MCF_hires_utc_now() = %.0f (delta %+.3f)\n", hi_now, hi_delta);
    assert(hi_delta >= -100);
    assert(hi_delta <= +100);
  }
