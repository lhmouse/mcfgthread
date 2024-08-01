/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_LIBCXX_IMPORT  __MCF_DLLEXPORT
#define __MCF_LIBCXX_INLINE  __MCF_DLLEXPORT
#define __MCF_LIBCXX_NO_ALIASES  1
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
