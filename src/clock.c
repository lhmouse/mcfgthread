/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#define __MCF_CLOCK_EXTERN_INLINE
#include "clock.h"
#include "win32.h"

int64_t
_MCF_utc_now(void)
  {
    /* Get the system time in NT epoch.
     * This is the number of 10^-7s since 1601-01-01T00:00:00Z.  */
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    ULARGE_INTEGER ui;
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;

    /* Convert it into Unix epoch in milliseconds.  */
    double nt_time = (double)(int64_t) ui.QuadPart;
    double unix_time = (nt_time - 116444736000000000) * 0.0001;
    return (int64_t) unix_time;
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
