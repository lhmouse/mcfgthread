/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_XWIN32_
#define __MCFGTHREAD_XWIN32_

#include "fwd.h"

/* Request Windows 7.  */
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

#ifndef __MCF_WIN32_EXTERN_INLINE
#  define __MCF_WIN32_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* Hard-code these.  */
#define GetCurrentProcess()  ((HANDLE) -1)
#define GetCurrentThread()   ((HANDLE) -2)

/* Add some attributes to existent functions.  */
DWORD __stdcall
GetCurrentThreadId(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

DWORD __stdcall
GetCurrentProcessId(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

DWORD __stdcall
GetLastError(void)
  __attribute__((__dllimport__, __nothrow__, __pure__));

VOID __stdcall
SetLastError(DWORD dwErrCode)
  __attribute__((__dllimport__, __nothrow__));

LPVOID __stdcall
TlsGetValue(DWORD dwTlsIndex)
  __attribute__((__dllimport__, __nothrow__, __pure__));

INT __stdcall
TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
  __attribute__((__dllimport__, __nothrow__));

HANDLE __stdcall
GetProcessHeap(void)
  __attribute__((__dllimport__, __nothrow__, __const__));

/* Declare some NTDLL functions that are not available here.  */
NTSTATUS __stdcall
NtWaitForKeyedEvent(HANDLE KeyedEvent, const void* Key, BOOLEAN Alertable, const LARGE_INTEGER* Timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtReleaseKeyedEvent(HANDLE KeyedEvent, const void* Key, BOOLEAN Alertable, const LARGE_INTEGER* Timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtDelayExecution(BOOLEAN Alertable, PLARGE_INTEGER Timeout)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout)
  __attribute__((__dllimport__, __nothrow__));

BOOLEAN __stdcall
RtlDllShutdownInProgress(void)
  __attribute__((__dllimport__, __nothrow__));

/* These are public APIs declared in the Windows DDK.  */
NTSTATUS __stdcall
NtDuplicateObject(HANDLE SourceProcessHandle, HANDLE SourceHandle, HANDLE TargetProcessHandle, HANDLE* TargetHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Options)
  __attribute__((__dllimport__, __nothrow__));

NTSTATUS __stdcall
NtClose(HANDLE Handle)
  __attribute__((__dllimport__, __nothrow__));

/* Declare helper functions here.  */
EXCEPTION_DISPOSITION __cdecl
__MCF_seh_top(EXCEPTION_RECORD* __record, void* __estab_frame, CONTEXT* __ctx, void* __disp_ctx) __MCF_NOEXCEPT;

#ifdef __i386__  /* SEH is stack-based  */

typedef struct __MCF_i386_seh_node __MCF_i386_seh_node;

struct __MCF_i386_seh_node
  {
    DWORD __next;  /* `__MCF_i386_seh_node*`  */
    DWORD __filter;  /* `typeof(__MCF_seh_top)*`  */
  };

__MCF_ALWAYS_INLINE
void
__MCF_i386_seh_install(__MCF_i386_seh_node* __seh_node) __MCF_NOEXCEPT
  {
    __seh_node->__next = __readfsdword(0U);
    __seh_node->__filter = (DWORD) __MCF_seh_top;
    __writefsdword(0U, (DWORD) __seh_node);
  }

__MCF_ALWAYS_INLINE
void
__MCF_i386_seh_cleanup(__MCF_i386_seh_node* __seh_node) __MCF_NOEXCEPT
  {
    __writefsdword(0U, __seh_node->__next);
  }

/* SEH is stack-based.  */
#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
      __MCF_i386_seh_node __MCF_PPCAT2(__MCF_seh_node_, __LINE__)  \
          __MCF_USE_DTOR(__MCF_i386_seh_cleanup);  \
      __MCF_i386_seh_install(  \
            &(__MCF_PPCAT2(__MCF_seh_node_, __LINE__)))  /* no semicolon  */

#else

/* SEH is table-based.  */
#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
      __asm__ volatile (".seh_handler __MCF_seh_top, @except;")  /* no semicolon  */

#endif

__MCF_ALWAYS_INLINE
NTSTATUS
__MCF_keyed_event_wait(const void* __key, const LARGE_INTEGER* __timeout) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtWaitForKeyedEvent(NULL, __key, false, __timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(__status));
    return __status;
  }

__MCF_ALWAYS_INLINE
NTSTATUS
__MCF_keyed_event_signal(const void* __key, const LARGE_INTEGER* __timeout) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtReleaseKeyedEvent(NULL, __key, false, __timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(__status));
    return __status;
  }

typedef struct __MCF_winnt_timeout __MCF_winnt_timeout;

struct __MCF_winnt_timeout
  {
    LARGE_INTEGER __li;
    uint64_t __since;
  };

void
__MCF_initialize_timeout_v2(__MCF_winnt_timeout* __to, const int64_t* __int64_opt) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void
__MCF_initialize_timeout_v2(__MCF_winnt_timeout* __to, const int64_t* __int64_opt) __MCF_NOEXCEPT
  {
    /* Initialize it to an infinite value.  */
    __to->__li.QuadPart = INT64_MIN;

    /* If no timeout is given, wait indefinitely.  */
    if(!__int64_opt)
      return;

    if(*__int64_opt > 0) {
      /* If `*__int64_opt` is positive, it denotes the number of milliseconds
       * since 1970-01-01T00:00:00Z, and has to be converted into the number of
       * 100 nanoseconds since the 1601-01-01T00:00:00Z.  */
      if(*__int64_opt > 910692730085477)
        return;

      __to->__li.QuadPart = (11644473600000 + *__int64_opt) * 10000;
      __to->__since = 0;
    }
    else if(*__int64_opt < 0) {
      /* If `*__int64_opt` is negative, it denotes the number of milliseconds
       * to wait, relatively.  */
      if(*__int64_opt < -922337203685477)
        return;

      __to->__li.QuadPart = *__int64_opt * 10000;
      __to->__since = GetTickCount64();
    }
    else
      __to->__li.QuadPart = 0;
  }

void
__MCF_adjust_timeout_v2(__MCF_winnt_timeout* __to) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void
__MCF_adjust_timeout_v2(__MCF_winnt_timeout* __to) __MCF_NOEXCEPT
  {
    /* Absolute timeouts need no adjustment.  */
    int64_t __temp = __to->__li.QuadPart;
    if(__temp >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    uint64_t __now = GetTickCount64();
    __temp += (int64_t) (__now - __to->__since) * 10000;
    __to->__li.QuadPart = __temp & (__temp >> 63);
    __to->__since = __now;
  }

/* Note this function is subject to tail-call optimization.  */
size_t
__MCF_batch_release_common(const void* __key, size_t __count) __MCF_NOEXCEPT;

/* Undefine macros that redirect to standard functions.
 * This ensures we call the ones from KERNEL32.  */
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

/* Copy a block of memory, like `memcpy()`.  */
void* __cdecl
__MCF_mcopy(void* __restrict__ __dst, const void* __restrict__ __src, size_t __size) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void* __cdecl
__MCF_mcopy(void* __restrict__ __dst, const void* __restrict__ __src, size_t __size) __MCF_NOEXCEPT
  {
    __MCFGTHREAD_ASSERT(__size <= (uintptr_t) __dst - (uintptr_t) __src);
#if defined(__i386__) || defined(__amd64__)
    typedef char __bytes[__size];
    uintptr_t __di, __si, __cx;
    __asm__ (
      "rep movsb;"
      : "=m"(*(__bytes*) __dst), "=D"(__di), "=S"(__si), "=c"(__cx)
      : "m"(*(const __bytes*) __src), "D"(__dst), "S"(__src), "c"(__size)
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlMoveMemory(__dst, __src, __size);
#endif
    return __dst;
  }

/* Copy a block of potentially overlapped memory, like `memmove()`.  */
void* __cdecl
__MCF_mmove(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void* __cdecl
__MCF_mmove(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    typedef char __bytes[__size];
    uintptr_t __di, __si, __cx;
    if(__size <= (uintptr_t) __dst - (uintptr_t) __src)
      __asm__ (
        "rep movsb;"  /* go forward  */
        : "=m"(*(__bytes*) __dst), "=D"(__di), "=S"(__si), "=c"(__cx)
        : "m"(*(const __bytes*) __src), "D"(__dst), "S"(__src), "c"(__size)
      );
    else
      __asm__ (
        "std;"
        "rep movsb;"  /* go backward  */
        "cld;"
        : "=m"(*(__bytes*) __dst), "=D"(__di), "=S"(__si), "=c"(__cx)
        : "m"(*(const __bytes*) __src), "D"((char*) __dst + __size - 1),
          "S"((const char*) __src + __size - 1), "c"(__size)
      );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlMoveMemory(__dst, __src, __size);
#endif
    return __dst;
  }

/* Fill a block of memory with the given byte, like `memset()`.  */
void* __cdecl
__MCF_mfill(void* __dst, uint8_t __val, size_t __size) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void* __cdecl
__MCF_mfill(void* __dst, uint8_t __val, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    typedef char __bytes[__size];
    uintptr_t __di, __cx;
    __asm__ (
      "rep stosb;"
      : "=m"(*(__bytes*) __dst), "=D"(__di), "=c"(__cx)
      : "D"(__dst), "a"(__val), "c"(__size)
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlFillMemory(__dst, __size, __val);
#endif
    return __dst;
  }

/* Fill a block of memory with zeroes, like `bzero()`.  */
void* __cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void* __cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    typedef char __bytes[__size];
    uintptr_t __di, __cx;
    __asm__ (
      "rep stosb;"
      : "=m"(*(__bytes*) __dst), "=D"(__di), "=c"(__cx)
      : "D"(__dst), "a"(0), "c"(__size)
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlZeroMemory(__dst, __size);
#endif
    return __dst;
  }

/* Compare two blocks of memory, like `memcmp()`.  */
int __cdecl
__MCF_mcomp(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_WIN32_EXTERN_INLINE
int __cdecl
__MCF_mcomp(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  {
    int __result;
#if defined(__i386__) || defined(__amd64__)
    typedef char __bytes[__size];
    uintptr_t __si, __di, __cx;
    __asm__ (
      "xorl %%eax, %%eax;"
      "repz cmpsb;"
      "setnzb %%al;"        /* EAX = 0 if equal; 1 if not equal.  */
      "sbbl %%ecx, %%ecx;"  /* ECX = 0 if equal or greater; -1 if less.  */
      : "=a"(__result), "=S"(__si), "=D"(__di), "=c"(__cx)
      : "m"(*(const __bytes*) __src), "m"(*(const __bytes*) __cmp),
        "S"(__src), "D"(__cmp), "c"(__size)
      : "cc"
    );
    __result |= (int) __cx;
#else
    /* Call the generic but slower version in NTDLL.  */
    SIZE_T __n = RtlCompareMemory(__src, __cmp, __size);
    if(__n != __size) {
      __result = *((const uint8_t*)__src + __n) - *((const uint8_t*)__cmp + __n);
      __MCFGTHREAD_ASSERT(__result != 0);
    }
    else
      __result = 0;
#endif
    return __result;
  }

/* Check whether two blocks of memory compare equal, like `memcmp() == 0`.
 * The result is a boolean value.  */
bool __cdecl
__MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_WIN32_EXTERN_INLINE
bool __cdecl
__MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  {
    bool __result;
#if defined(__i386__) || defined(__amd64__)
    typedef char __bytes[__size];
    uintptr_t __si, __di, __cx;
    __asm__ (
      "xorl %%eax, %%eax;"
      "repz cmpsb;"
#  ifdef __GCC_ASM_FLAG_OUTPUTS__
      /* Store the result in FL and clobber AX.  */
      : "=@ccz"(__result),
#  else
      /* Store the result in AX and clobber FL.  */
      "setzb %%al;"
      : "=a"(__result),
#  endif
        "=S"(__si), "=D"(__di), "=c"(__cx)
      : "m"(*(const __bytes*) __src), "m"(*(const __bytes*) __cmp),
        "S"(__src), "D"(__cmp), "c"(__size)
#  ifdef __GCC_ASM_FLAG_OUTPUTS__
      : "ax"
#  else
      : "cc"
#  endif
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    SIZE_T __n = RtlCompareMemory(__src, __cmp, __size);
    __result = __n == __size;
#endif
    return __result;
  }

/* Allocate a block of zeroed memory, like `calloc()`.  */
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __malloc__, __alloc_size__(1)));

__MCF_WIN32_EXTERN_INLINE
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, __size);
    return __ptr;
  }

/* Re-allocate a block of memory, like `realloc()`. If the existent
 * block should be extended, vacuum bytes are filled with zeroes.  */
void*
__MCF_mrealloc_0(void** __restrict__ __pptr, size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __alloc_size__(2)));

__MCF_WIN32_EXTERN_INLINE
void*
__MCF_mrealloc_0(void** __restrict__ __pptr, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *__pptr, __size);
    return !__ptr ? NULL : (*__pptr = __ptr);
  }

/* Allocate a copy of a block of memory, like `malloc()` followed by
 * `memcpy()`.  */
void*
__MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __alloc_size__(2)));

__MCF_WIN32_EXTERN_INLINE
void*
__MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapAlloc(GetProcessHeap(), 0, __size);
    return !__ptr ? NULL : __MCF_mcopy(__ptr, __data, __size);
  }

/* Get the size of an allocated block, like `malloc_usable_size()`.  */
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_WIN32_EXTERN_INLINE
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  {
    size_t __size = HeapSize(GetProcessHeap(), 0, __ptr);
    __MCFGTHREAD_ASSERT(__size != (size_t)-1);
    return __size;
  }

/* Free a block of memory, like `free()`.  */
void
__MCF_mfree(void* __ptr) __MCF_NOEXCEPT;

__MCF_WIN32_EXTERN_INLINE
void
__MCF_mfree(void* __ptr) __MCF_NOEXCEPT
  {
    if(!__ptr)
      return;

#ifdef __MCF_DEBUG
    __MCF_mfill(__ptr, 0xFE, HeapSize(GetProcessHeap(), 0, __ptr));
#endif
    int __succ = HeapFree(GetProcessHeap(), 0, __ptr);
    __MCFGTHREAD_ASSERT(__succ);
  }

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_XWIN32_  */
