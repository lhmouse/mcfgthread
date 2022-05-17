/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_CLOCK(...)  __MCF_DLLEXPORT
#include "clock.h"
#include "xglobals.i"

static inline
double
do_unix_time_from_nt_time(const FILETIME* ft)
  {
    /* Sum the high and low parts.  */
    LARGE_INTEGER li;
    li.LowPart = ft->dwLowDateTime;
    li.HighPart = (LONG) ft->dwHighDateTime;

    /* 11644473600000 is number of milliseconds from 1601-01-01T00:00:00Z
     * (the NT epoch) to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    return (double) li.QuadPart * 0.0001 - 11644473600000;
  }

int64_t
_MCF_utc_now(void)
  {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (int64_t) do_unix_time_from_nt_time(&ft);
  }

double
_MCF_hires_utc_now(void)
  {
    FILETIME ft;
#if _WIN32_WINNT >= 0x0602
    GetSystemTimePreciseAsFileTime(&ft);
#else
    GetSystemTimeAsFileTime(&ft);
#endif
    return do_unix_time_from_nt_time(&ft);
  }

int64_t
_MCF_tick_count(void)
  {
    return (int64_t) GetTickCount64();
  }

double
_MCF_perf_counter(void)
  {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double) li.QuadPart * __MCF_perf_frequency_reciprocal;
  }
