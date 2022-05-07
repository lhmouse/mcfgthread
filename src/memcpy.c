/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#include "xwin32.i"

void* __cdecl
memcpy(void* dst, const void* src, size_t size)
  {
    return __MCF_mcopy(dst, src, size);
  }
