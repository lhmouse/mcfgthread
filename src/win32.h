// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_WIN32_H_
#define __MCFGTHREAD_WIN32_H_

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
#include <ntstatus.h>
#include <winternl.h>

// Undefine macros that redirect to standard functions.
// This ensures we call the ones from KERNEL32.
#undef RtlCopyMemory
#undef RtlMoveMemory
#undef RtlFillMemory
#undef RtlZeroMemory
#undef RtlCompareMemory
#undef RtlEqualMemory

void __stdcall
RtlMoveMemory(void* __dst, const void* __src, SIZE_T __size)
  __attribute__((__dllimport__, __nothrow__));

void __stdcall
RtlFillMemory(void* __dst, SIZE_T __size, int __val)
  __attribute__((__dllimport__, __nothrow__));

void __stdcall
RtlZeroMemory(void* __dst, SIZE_T __size)
  __attribute__((__dllimport__, __nothrow__));

SIZE_T __stdcall
RtlCompareMemory(const void* __s1, const void* __s2, SIZE_T __size)
  __attribute__((__dllimport__, __pure__, __nothrow__));

// Declare some NTDLL functions that are not available here.
NTSTATUS __stdcall
NtWaitForKeyedEvent(HANDLE __event, void* __key, BOOLEAN __alertable, const LARGE_INTEGER* __timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtReleaseKeyedEvent(HANDLE __event, void* __key, BOOLEAN __alertable, const LARGE_INTEGER* __timeout)
  __attribute__((__dllimport__, __nothrow__));

BOOLEAN __stdcall
RtlDllShutdownInProgress(void)
  __attribute__((__dllimport__, __nothrow__));

#endif  // __MCFGTHREAD_WIN32_H_
