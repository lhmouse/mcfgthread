/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
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

/* With Clang, `-masm=intel` doesn't affect module-level assembly, and requires
 * this workaround.  */
#if defined __clang__ && (defined __i386__ || (defined __amd64__ && !defined __arm64ec__))
__asm__ (".intel_syntax noprefix");
#endif

/* Define internal build options, and additionally, work around many problems
 * about Clang-CL and the MSVC standard library.  */
#ifdef __MCF_IN_DLL
#  define __MCF_DLLEXPORT  __declspec(dllexport)
#else
#  define __MCF_DLLEXPORT
#endif

/* When building the DLL, GCC may make implicit calls to these functions, so
 * we must define and export them. However, Clang requires that the `dllexport`
 * attribute be applied before a function is called, so declare them as early as
 * possible.  */
__MCF_DLLEXPORT void* memcpy(void* __dst, const void* __src, size_t __size);
__MCF_DLLEXPORT void* memmove(void* __dst, const void* __src, size_t __size);
__MCF_DLLEXPORT void* memset(void* __dst, int __val, size_t __size);
__MCF_DLLEXPORT int memcmp(const void* __src, const void* __dst, size_t __size);

#define WIN32_LEAN_AND_MEAN  1
#define NOMINMAX  1
#define NOGDI  1
#define NOMSG  1
#ifndef _WIN32_WINNT
#  define _WIN32_WINNT  0x0601
#endif
#include <windows.h>

#define __MCF_EXPAND_INLINE_DEFINITIONS   1

#endif  /* __MCFGTHREAD_XPRECOMPILED_  */
