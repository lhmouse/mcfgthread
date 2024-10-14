/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#pragma GCC diagnostic ignored "-Wmissing-declarations"

int
__cdecl
__MCF_mcompare(const void* src, const void* cmp, size_t size);

__MCF_DLLEXPORT
int
__cdecl
memcmp(const void* src, const void* cmp, size_t size)
  {
    return __MCF_mcompare(src, cmp, size);
  }
