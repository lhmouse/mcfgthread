/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#include "xwin32.i"

void* __cdecl
memmove(void* dst, const void* src, size_t size)
  {
    return __MCF_mmove(dst, src, size);
  }
