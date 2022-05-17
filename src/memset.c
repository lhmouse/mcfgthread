/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"

void* __cdecl
__MCF_mfill(void* dst, int val, size_t size);

void* __cdecl
memset(void* dst, int val, size_t size);

__MCF_DLLEXPORT
void* __cdecl
memset(void* dst, int val, size_t size)
  {
    return __MCF_mfill(dst, val, size);
  }
