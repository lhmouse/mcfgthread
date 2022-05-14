/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_PRECOMPILED_
#define __MCFGTHREAD_PRECOMPILED_

#include "version.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef DLL_EXPORT
#  define __MCF_DLLEXPORT  __declspec(dllexport)
#else
#  define __MCF_DLLEXPORT
#endif

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT  _WIN32_WINNT_WIN7
#endif
#if _WIN32_WINNT < _WIN32_WINNT_WIN7
#  error Please define `_WIN32_WINNT` to at least Windows 7.
#endif
#define WIN32_LEAN_AND_MEAN  1
#define NOMINMAX  1
#define NOCOMM 1
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <limits.h>

#endif  /* __MCFGTHREAD_PRECOMPILED_  */
