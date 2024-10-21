/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#pragma GCC diagnostic ignored "-Wmissing-declarations"

void*
__cdecl
__MCF_mfill(void* dst, int val, size_t size);

__MCF_DLLEXPORT
void*
__cdecl
memset(void* dst, int val, size_t size)
  {
    return __MCF_mfill(dst, val, size);
  }
