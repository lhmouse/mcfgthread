/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_LIBCXX_IMPORT  __MCF_DLLEXPORT
#define __MCF_LIBCXX_INLINE  __MCF_DLLEXPORT
#define __MCF_LIBCXX_NO_ALIASES  1
#include "../libcxx.h"

__MCF_DLLEXPORT
void
__MCF_libcxx_thread_sleep_for_ns_count(int64_t ns)
  {
    double ms = 0.9999999;
    ms += (double) ns * 0.000001;
    ms = __builtin_fmax(ms, 0);
    ms = __builtin_fmin(ms, 0x7FFFFFFFFFFFFC00);
    _MCF_sleep_noninterruptible(&(int64_t){ (int64_t) -ms });
  }
