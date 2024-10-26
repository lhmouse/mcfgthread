/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_XPRECOMPILED_
#define __MCFGTHREAD_XPRECOMPILED_

#include "version.h"

#ifdef __MCF_BUILDING_DLL
#  define __MCF_DLLEXPORT  __declspec(dllexport)
#else
#  define __MCF_DLLEXPORT
#endif

#include <minwindef.h>
#include <winnt.h>
#include <winternl.h>
#include <winerror.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <intrin.h>

#endif  /* __MCFGTHREAD_XPRECOMPILED_  */
