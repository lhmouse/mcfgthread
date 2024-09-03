/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#define _POSIX_C_SOURCE 200809
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

int
main(void)
  {
#if defined _WIN32
    _tzset();
#else
    tzset();
#endif

    SYSTEMTIME st;
    GetLocalTime(&st);
    struct tm tm;
    tm.tm_year = st.wYear - 1900;
    tm.tm_mon = st.wMonth - 1;
    tm.tm_mday = st.wDay;
    tm.tm_hour = st.wHour;
    tm.tm_min = st.wMinute;
    tm.tm_sec = st.wSecond;
    tm.tm_isdst = -1;
#if defined _WIN32 && defined _USE_32BIT_TIME_T
    __time64_t ts = _mktime64(&tm);
#else
    time_t ts = mktime(&tm);
#endif

    long long vcrt_now = ts * 1000 + st.wMilliseconds;
    printf("vcrt_now = %lld\n", vcrt_now);

    long long ms_now = _MCF_utc_now();
    long long ms_delta = ms_now - vcrt_now;
    printf("_MCF_utc_now() = %lld (delta %+lld)\n", ms_now, ms_delta);
    assert(ms_delta >= -100);
    assert(ms_delta <= +100);

    double hi_now = _MCF_hires_utc_now();
    double hi_delta = hi_now - (double) vcrt_now;
    printf("_MCF_hires_utc_now() = %.0f (delta %+.3f)\n", hi_now, hi_delta);
    assert(hi_delta >= -100);
    assert(hi_delta <= +100);
  }
