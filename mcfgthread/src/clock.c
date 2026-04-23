/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_CLOCK_IMPORT  __MCF_DLLEXPORT
#define __MCF_CLOCK_INLINE  __MCF_DLLEXPORT
#include "../clock.h"
#include "xglobals.h"

static inline
uint64_t
do_divide_by_10000(uint64_t value)
  {
#if defined __SIZEOF_INT128__
    return (uint64_t) ((unsigned __int128) value * 0x68DB8BAC710CCULL >> 64);
#elif defined _MSC_VER && __MCF_64_32(1, 0)
    return __umulh(value, 0x68DB8BAC710CCULL);
#else
    uint64_t x0 = (uint32_t) value * 0xBAC710CCULL >> 32;
    uint64_t x1 = (uint32_t) value * 0x68DB8ULL;
    uint64_t x2 = ((value >> 32) * 0xBAC710CCULL + (uint32_t) x1 + x0) >> 32;
    return (value >> 32) * 0x68DB8ULL + x2 + (x1 >> 32);
#endif
  }

__MCF_DLLEXPORT
int64_t
_MCF_utc_now(void)
  {
    /* `ULONGLONG` has a more strict alignment requirement than `FILETIME`,
     * so the cast should be safe. `116444736000000000` is number of 1/100
     * seconds from 1601-01-01T00:00:00Z (the NT epoch) to 1970-01-01T00:00:00Z
     * (the Unix Epoch).  */
    ULONGLONG ull;
    GetSystemTimeAsFileTime((FILETIME*) &ull);
    return (int64_t) do_divide_by_10000(ull - 116444736000000000);
  }

__MCF_DLLEXPORT
double
_MCF_hires_utc_now(void)
  {
    if(__MCF_HAS_G_IMP(GetSystemTimePreciseAsFileTime)) {
      /* This is available since Windows 8.  */
      ULONGLONG ull;
      __MCF_G_IMP(GetSystemTimePreciseAsFileTime) ((FILETIME*) &ull);
      return (double)(int64_t) (ull - 116444736000000000) * 0.0001;
    }

    /* `ULONGLONG` has a more strict alignment requirement than `FILETIME`,
     * so the cast should be safe. `116444736000000000` is number of 1/100
     * seconds from 1601-01-01T00:00:00Z (the NT epoch) to 1970-01-01T00:00:00Z
     * (the Unix Epoch).  */
    ULONGLONG ull;
    GetSystemTimeAsFileTime((FILETIME*) &ull);
    return (double)(int64_t) (ull - 116444736000000000) * 0.0001;
  }

__MCF_DLLEXPORT
int64_t
_MCF_steady_now(void)
  {
    /* This is available since Windows 7.  */
    ULONGLONG ull;
    QueryUnbiasedInterruptTime(&ull);
    return (int64_t) do_divide_by_10000(ull);
  }

__MCF_DLLEXPORT
double
_MCF_hires_steady_now(void)
  {
    if(__MCF_HAS_G_IMP(QueryUnbiasedInterruptTimePrecise)) {
      /* This is available since Windows 10.  */
      ULONGLONG ull;
      __MCF_G_IMP(QueryUnbiasedInterruptTimePrecise) (&ull);
      return (double)(int64_t) ull * 0.0001;
    }

    /* This is available since Windows 7.  */
    ULONGLONG ull;
    QueryUnbiasedInterruptTime(&ull);
    return (double)(int64_t) ull * 0.0001;
  }

static inline
void
do_QueryInterruptTime(ULONGLONG* outp)
  {
    /* At address `0x7FFE0000` (`MM_SHARED_USER_DATA_VA` in Windows SDK for
     * assembly, same on all architectures) there's a read-only structure of
     * type `KUSER_SHARED_DATA`. The `InterruptTime` field is at offset `8` on
     * all architectures; see
     * <https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data>.  */
    volatile char* shared_user_data = (char*) 0x7FFE0000;
    __asm__ ("" : "+r"(shared_user_data));  /* workaround for optimizer bug  */
#if __MCF_64_32(1, 0)
    *outp = *(volatile ULONGLONG*) (shared_user_data + 8);
#else
    ULONG high, low;
    do {
      /* A 32-bit kernel does not write 64-bit integers atomically, which
       * requires special treatment. The kernel writes `High2Time` then
       * `LowPart` then `High1Time` so we read them in the reverse order. If
       * `High1Time` does not equal `High2Time`, then the value will have
       * been split and we must try again.  */
      high = *(volatile ULONG*) (shared_user_data + 12);
      low = *(volatile ULONG*) (shared_user_data + 8);
    } while(high != *(volatile ULONG*) (shared_user_data + 16));
    *outp = (ULONGLONG) high << 32 | low;
#endif
  }


__MCF_DLLEXPORT
int64_t
_MCF_tick_count(void)
  {
    /* The interrupt-time count is more precise than `GetTickCount64()`.  */
    ULONGLONG ull;
    do_QueryInterruptTime(&ull);
    return (int64_t) do_divide_by_10000(ull);
  }

__MCF_DLLEXPORT
double
_MCF_hires_tick_count(void)
  {
    if(__MCF_HAS_G_IMP(QueryInterruptTimePrecise)) {
      /* This is available since Windows 10.  */
      ULONGLONG ull;
      __MCF_G_IMP(QueryInterruptTimePrecise) (&ull);
      return (double)(int64_t) ull * 0.0001;
    }

    /* The interrupt-time count is more precise than `GetTickCount64()`.  */
    ULONGLONG ull;
    do_QueryInterruptTime(&ull);
    return (double)(int64_t) ull * 0.0001;
  }

__MCF_DLLEXPORT
double
_MCF_perf_counter(void)
  {
    /* This will not fail since Windows XP.  */
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double) li.QuadPart * __MCF_crt_pf_recip;
  }
