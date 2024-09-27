/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#include <string.h>

void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size);

__MCF_DLLEXPORT
void*
__cdecl
memcpy(void* dst, const void* src, size_t size)
  {
    return __MCF_mcopy(dst, src, size);
  }
