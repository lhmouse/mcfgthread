// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_TIME_C_  1
#include "time.h"
#include "win32.h"

int64_t
_MCF_utc_now(void)
  {
    // Get the system time in NT epoch.
    // This is the number of 10,000,000th second since 1601-01-01T00:00:00Z.
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    ULARGE_INTEGER ui;
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;

    // Convert it into Unix epoch in milliseconds.
    return (int64_t) (((double)(int64_t) ui.QuadPart - 116444736e9) / 1e4);
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
    return (double) li.QuadPart * _MCF_perf_frequency_reciprocal;
  }
