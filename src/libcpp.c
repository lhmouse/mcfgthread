/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_LIBCPP(...)  __MCF_DLLEXPORT
#include "libcpp.h"

__MCF_DLLEXPORT
void
__MCF_libcpp_thread_sleep_for(double ns)
  {
    /* Clamp the timestamp.  */
    double value = 0.0009999 + ns;
    value = __builtin_fmax(value, 0);
    value = __builtin_fmin(value, 0x1p63 - 0x1p10);

    int64_t timeout = (int64_t) value;
    _MCF_sleep(&timeout);
  }
