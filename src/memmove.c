/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"

void* __cdecl
__MCF_mmove(void* dst, const void* src, size_t size);

void* __cdecl
memmove(void* dst, const void* src, size_t size);

__MCF_DLLEXPORT
void* __cdecl
memmove(void* dst, const void* src, size_t size)
  {
    return __MCF_mmove(dst, src, size);
  }
