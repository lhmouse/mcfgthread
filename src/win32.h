// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_WIN32_H_
#define __MCFGTHREAD_WIN32_H_

#include "fwd.h"

// Request Windows 7.
#ifndef _WIN32_WINNT
#  define _WIN32_WINNT  0x0601
#endif

#if _WIN32_WINNT < 0x0601
#  error Please define `_WIN32_WINNT` to at least Windows 7.
#endif

#define WIN32_LEAN_AND_MEAN  1
#define NOMINMAX  1
#define NOCOMM 1
#include <windows.h>
#include <ntstatus.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MCFGTHREAD_WIN32_C_
#  define __MCFGTHREAD_WIN32_INLINE
#else
#  define __MCFGTHREAD_WIN32_INLINE  __MCF_GNU_INLINE
#endif

// Add some attributes to existent functions.
DWORD __stdcall
GetCurrentThreadId(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

HANDLE __stdcall
GetCurrentThread(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

DWORD __stdcall
GetCurrentProcessId(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

HANDLE __stdcall
GetCurrentProcess(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

DWORD __stdcall
GetLastError(void)
  __attribute__((__dllimport__, __nothrow__, __pure__));

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
NtWaitForKeyedEvent(HANDLE __event, const void* __key, BOOLEAN __alertable, LARGE_INTEGER* __timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtReleaseKeyedEvent(HANDLE __event, const void* __key, BOOLEAN __alertable, LARGE_INTEGER* __timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtDelayExecution(BOOLEAN __alertable, LARGE_INTEGER* __timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtWaitForSingleObject(HANDLE __object, BOOLEAN __alertable, LARGE_INTEGER* __timeout)
  __attribute__((__dllimport__, __nothrow__));

BOOLEAN __stdcall
RtlDllShutdownInProgress(void)
  __attribute__((__dllimport__, __nothrow__));

// Declare helper functions here.
EXCEPTION_DISPOSITION __cdecl
__MCF_seh_top(EXCEPTION_RECORD* __record, void* __frame, CONTEXT* __ctx, void* __disp_ctx)
  __attribute__((__nothrow__));

#ifdef __i386__  // SEH is stack-based

struct __MCF_i386_seh_node
  {
    void* __next;
    void* __filter;  // typeof(__MCF_seh_top)*
  }
  typedef __MCF_i386_seh_node;

static __attribute__((__always_inline__))
__inline__ void
__MCF_i386_seh_install(__MCF_i386_seh_node* __seh_node) __MCF_NOEXCEPT
  {
    __asm__ (
      "movl %%fs:0, %%eax            \n"//  mov eax, fs:[0]
      "movl %%eax, (%0)              \n"//  mov [%0], eax
      "movl $___MCF_seh_top, 4(%0)   \n"//  mov [%0 + 4], offset FILTER
      "movl %0, %%fs:0               \n"//  mov fs:[0], %0
      : : "r"(__seh_node)
      : "eax", "memory");  // EAX is unlikely a parameter
  }

static __attribute__((__always_inline__))
__inline__ void
__MCF_i386_seh_cleanup(__MCF_i386_seh_node* __seh_node) __MCF_NOEXCEPT
  {
    __asm__ (
      "movl %0, %%ecx                \n"//  mov ecx, [%0]
      "movl %%ecx, %%fs:0            \n"//  mov fs:[0], ecx
      : : "m"(*__seh_node)
      : "ecx", "memory");  // ECX is unlikely a return value
  }

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __MCF_i386_seh_node __MCF_seh_node_1 __MCF_USE_DTOR(__MCF_i386_seh_cleanup);  \
    __MCF_i386_seh_install(&__MCF_seh_node_1)  // no semicolon

#else  // SEH is stack-based  ^/v  SEH is table-based

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __asm__ volatile (".seh_handler __MCF_seh_top, @except")  // no semicolon

#endif  // SEH is table-based

LARGE_INTEGER*
__MCF_initialize_timeout(LARGE_INTEGER* __li, const int64_t* __int64_opt)
  __attribute__((__nothrow__));

__MCFGTHREAD_WIN32_INLINE LARGE_INTEGER*
__MCF_initialize_timeout(LARGE_INTEGER* __li, const int64_t* __int64_opt)
  {
    double __nt_time;

    if(__int64_opt == NULL)
      return NULL;  // wait infinitely

    if(*__int64_opt > 0) {
      // `*__int64_opt` is milliseconds since 1970-01-01T00:00:00Z.
      // Convert it into the NT epoch.
      __nt_time = 116444736000000000 + (double) *__int64_opt * 10000;
      if(__nt_time > 0x7FFFFFFFFFFFFC00)
        return NULL;  // overflowed; assume infinity

      __li->QuadPart = (int64_t) __nt_time;
      return __li;
    }

    if(*__int64_opt < 0) {
      //  `*__int64_opt` is milliseconds to wait.
      __nt_time = (double) *__int64_opt * 10000;
      if(__nt_time < -0x7FFFFFFFFFFFFC00)
        return NULL;  // overflowed; assume infinity

      __li->QuadPart = (int64_t) __nt_time;
      return __li;
    }

    // The wait shall time out immediately.
    __li->QuadPart = 0;
    return __li;
  }

size_t
__MCF_batch_release_common(const void* __key, size_t __count)
  __attribute__((__nothrow__));

__MCFGTHREAD_WIN32_INLINE size_t
__MCF_batch_release_common(const void* __key, size_t __count)
  {
    size_t __k;

    if(__count == 0)
      return 0;

    // A call to `ExitProcess()` terminates all the other threads, even
    // if they are waiting. Don't release the keyed event in this case,
    // as it blocks the calling thread infinitely if there is no thread
    // to wake up. See <https://github.com/lhmouse/mcfgthread/issues/21>.
    if(RtlDllShutdownInProgress())
      return 0;

    for(__k = 0;  __k != __count;  ++__k) {
      // Release a thread. This operation shall block until the target
      // thread has received the notification.
      NTSTATUS __status = NtReleaseKeyedEvent(NULL, __key, FALSE, NULL);
      __MCFGTHREAD_ASSERT(NT_SUCCESS(__status));
    }

    // Return the number of threads that have been woken.
    return __count;
  }

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_WIN32_H_
