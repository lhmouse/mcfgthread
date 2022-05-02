/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#include "win32.h"

int __cdecl
memcmp(const void* src, const void* cmp, size_t size)
  {
    return __MCF_mcomp(src, cmp, size);
  }
