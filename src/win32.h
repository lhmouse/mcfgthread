// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCF_WIN32_H_
#define __MCF_WIN32_H_

// Request Windows Vista.
#ifndef _WIN32_WINNT
#  define _WIN32_WINNT  0x0600
#endif

#if _WIN32_WINNT < 0x0600
#  error Please define `_WIN32_WINNT` to at least Windows Vista.
#endif

#define WIN32_LEAN_AND_MEAN  1
#define NOMINMAX  1
#define NOCOMM 1
#include <windows.h>

// Declare NTDLL functions that is not available here.

#endif  // __MCF_WIN32_H_
