/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"

void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size);

void*
__cdecl
__MCF_mcopy_backward(void* dst, const void* src, size_t size);

__MCF_DLLEXPORT
void*
__cdecl
memmove(void* dst, const void* src, size_t size);

__MCF_DLLEXPORT
void*
__cdecl
memmove(void* dst, const void* src, size_t size)
  {
    return ((uintptr_t) dst - (uintptr_t) src >= size)
        ? __MCF_mcopy(dst, src, size)
        : __MCF_mcopy_backward(dst, src, size);
  }
