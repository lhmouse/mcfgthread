/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_XPRECOMPILED_
#define __MCFGTHREAD_XPRECOMPILED_

#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

/* Define internal build options, and additionally, work around many problems
 * about Clang-CL and the MSVC standard library.  */
#ifdef __MCF_BUILDING_DLL
#  define __MCF_DLLEXPORT  __declspec(dllexport)
#else
#  define __MCF_DLLEXPORT
#endif

__MCF_DLLEXPORT void* __cdecl memcpy(void* __dst, const void* __src, size_t __size);
__MCF_DLLEXPORT void* __cdecl memmove(void* __dst, const void* __src, size_t __size);
__MCF_DLLEXPORT int __cdecl memcmp(const void* __src, const void* __cmp, size_t __size);
__MCF_DLLEXPORT void* __cdecl memset(void* __dst, int val, size_t __size);

#include <minwindef.h>
#include <winnt.h>
#include <winternl.h>
#include <winerror.h>

#define __MCF_EXPAND_INLINE_DEFINITIONS   1

#endif  /* __MCFGTHREAD_XPRECOMPILED_  */
