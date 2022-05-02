/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#include "win32.h"

void* __cdecl
memset(void* dst, int val, size_t size)
  {
    return __MCF_mfill(dst, (uint8_t) val, size);
  }
