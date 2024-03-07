/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#include "xprecompiled.h"
#define __MCF_CLOCK_IMPORT  __MCF_DLLEXPORT
#define __MCF_CLOCK_INLINE  __MCF_DLLEXPORT
#include "clock.h"
#include "xglobals.h"

static inline
uint64_t
do_divide_by_10000(uint64_t value)
  {
#ifdef __SIZEOF_INT128__
    return (uint64_t) ((unsigned __int128) value * 0x68DB8BAC710CBULL >> 64);
#else
    uint64_t temp = (uint32_t) value * 0xBAC710CBULL >> 32;
    uint64_t middle = (uint32_t) value * 0x68DB8ULL;
    temp = ((value >> 32) * 0xBAC710CBULL + (uint32_t) middle + temp) >> 32;
    return temp + (middle >> 32) + (value >> 32) * 0x68DB8ULL;
#endif
  }

__MCF_DLLEXPORT
int64_t
_MCF_utc_now(void)
  {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    /* 11644473600000 is number of milliseconds from 1601-01-01T00:00:00Z
     * (the NT epoch) to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    uint64_t t = ft.dwLowDateTime;
    t += (uint64_t) ft.dwHighDateTime << 32;
    t = do_divide_by_10000(t);
    return (int64_t) (t - 11644473600000);
  }

__MCF_DLLEXPORT
double
_MCF_hires_utc_now(void)
  {
    __MCF_LAZY_DECLARE(GetSystemTimePreciseAsFileTime);
    FILETIME ft;

    /* This is available since Windows 8.  */
    if(__MCF_LAZY_GET(GetSystemTimePreciseAsFileTime))
      __MCF_LAZY_DEREF(GetSystemTimePreciseAsFileTime) (&ft);
    else
      GetSystemTimeAsFileTime(&ft);

    /* 11644473600000 is number of milliseconds from 1601-01-01T00:00:00Z
     * (the NT epoch) to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    double t = (double) ft.dwLowDateTime;
    t += (double) ft.dwHighDateTime * 0x1p32;
    t *= 0.0001;
    return t - 11644473600000;
  }

__MCF_DLLEXPORT
int64_t
_MCF_tick_count(void)
  {
    return (int64_t) GetTickCount64();
  }

__MCF_DLLEXPORT
double
_MCF_perf_counter(void)
  {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double) li.QuadPart * __MCF_crt_pf_recip;
  }
