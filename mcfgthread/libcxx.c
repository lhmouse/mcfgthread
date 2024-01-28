/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#include "precompiled.h"
#define __MCF_LIBCXX_IMPORT  __MCF_DLLEXPORT
#define __MCF_LIBCXX_INLINE  __MCF_DLLEXPORT
#include "libcxx.h"

__MCF_DLLEXPORT
void
__MCF_libcxx_thread_sleep_for_ns_count(int64_t ns)
  {
    double value = 0.9999999;
    value += (double) ns * 0.000001;

    /* Clamp the timeout.  */
    value = __builtin_fmax(value, 0);
    value = __builtin_fmin(value, 0x1p63 - 0x1p10);

    int64_t timeout = (int64_t) -value;
    _MCF_sleep_noninterruptible(&timeout);
  }
