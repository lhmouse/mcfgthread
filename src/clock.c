/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_CLOCK(...)  __MCF_DLLEXPORT
#include "clock.h"
#include "xglobals.i"

int64_t
_MCF_utc_now(void)
  {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    /* `t := (low + high * 0x1p32) * 0x68DB8`BAC710CB / 0x1p64`  */
    uint64_t t = ft.dwLowDateTime * 0xBAC710CBULL >> 32;
    uint64_t middle = ft.dwLowDateTime * 0x68DB8ULL;
    t = (ft.dwHighDateTime * 0xBAC710CBULL + (uint32_t) middle + t) >> 32;
    t += (middle >> 32) + ft.dwHighDateTime * 0x68DB8ULL;

    /* 11644473600000 is number of milliseconds from 1601-01-01T00:00:00Z
     * (the NT epoch) to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    return (int64_t) t - 11644473600000;
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

    /* `t := (low + high * 0x1p32) / 10000`  */
    double t = (double) ft.dwLowDateTime;
    t += (double) ft.dwHighDateTime * 0x1p32;
    t *= 0.0001;

    /* 11644473600000 is number of milliseconds from 1601-01-01T00:00:00Z
     * (the NT epoch) to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    return t - 11644473600000;
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
