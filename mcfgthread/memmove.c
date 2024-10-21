/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"

void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size);

void*
__cdecl
__MCF_mcopy_backward(void* dst, const void* src, size_t size);

__MCF_DLLEXPORT
void*
__cdecl
memmove(void* dst, const void* src, size_t size)
  {
    return ((uintptr_t) dst - (uintptr_t) src >= size)
        ? __MCF_mcopy(dst, src, size)
        : __MCF_mcopy_backward(dst, src, size);
  }
