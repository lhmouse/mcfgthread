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

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT  0x0601
#endif

#define NOMINMAX  1
#define NOGDI  1
#define NOMSG  1
#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>

#define __MCF_EXPAND_INLINE_DEFINITIONS   1

#endif  /* __MCFGTHREAD_XPRECOMPILED_  */
