/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_XGLOBALS_
#define __MCFGTHREAD_XGLOBALS_

#include "fwd.h"
#include "xasm.i"
#include <minwindef.h>
#include <ntdef.h>
#include <winerror.h>
#include <ntstatus.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32_WINNT
#  error Only Windows platforms are supported.
#endif

#if _WIN32_WINNT < 0x0601
#  error Please define `_WIN32_WINNT` to at least Windows 7.
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

#ifndef __MCF_DECLSPEC_XGLOBALS
#  define __MCF_DECLSPEC_XGLOBALS(...)  __VA_ARGS__
#endif

/* Declare global data.  */
extern DWORD __MCF_win32_tls_index;
extern double __MCF_perf_frequency_reciprocal;
extern _MCF_thread __MCF_main_thread;

extern _MCF_mutex __MCF_cxa_atexit_mutex;
extern __MCF_dtor_queue __MCF_cxa_atexit_queue;
extern _MCF_mutex __MCF_cxa_at_quick_exit_mutex;
extern __MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;
extern _MCF_cond __MCF_interrupt_cond;
extern BYTE __MCF_mutex_spin_field[2048];

/* Hard-code these.  */
#define GetCurrentProcess()  ((HANDLE) -1)
#define GetCurrentThread()   ((HANDLE) -2)

/* Undefine macros that redirect to standard functions.
 * This ensures we call the ones from KERNEL32.  */
#undef RtlCopyMemory
#undef RtlMoveMemory
#undef RtlFillMemory
#undef RtlZeroMemory
#undef RtlCompareMemory
#undef RtlEqualMemory

#define __MCF_WINAPI(RETURN, function, ...)  \
  RETURN __stdcall function(__VA_ARGS__)  \
    __attribute__((__dllimport__, __nothrow__))

#define __MCF_CHECK_NT(...)  \
  __MCF_CHECK(NT_SUCCESS(__VA_ARGS__))

/* Declare KERNEL32 APIs here.  */
__MCF_WINAPI(DWORD, GetLastError, void) __attribute__((__pure__));
__MCF_WINAPI(void, SetLastError, DWORD);

__MCF_WINAPI(DWORD, GetCurrentThreadId, void) __attribute__((__const__));
__MCF_WINAPI(DWORD, GetCurrentProcessId, void) __attribute__((__const__));

__MCF_WINAPI(DWORD, TlsAlloc, void);
__MCF_WINAPI(BOOL, TlsFree, DWORD);
__MCF_WINAPI(LPVOID, TlsGetValue, DWORD) __attribute__((__pure__));
__MCF_WINAPI(BOOL, TlsSetValue, DWORD, LPVOID);

__MCF_WINAPI(HANDLE, GetProcessHeap, void) __attribute__((__const__));
__MCF_WINAPI(LPVOID, HeapAlloc, HANDLE, DWORD, SIZE_T) __attribute__((__alloc_size__(3)));
__MCF_WINAPI(LPVOID, HeapReAlloc, HANDLE, DWORD, LPVOID, SIZE_T) __attribute__((__alloc_size__(4)));
__MCF_WINAPI(SIZE_T, HeapSize, HANDLE, DWORD, LPCVOID) __attribute__((__pure__));
__MCF_WINAPI(BOOL, HeapFree, HANDLE, DWORD, LPVOID);

__MCF_WINAPI(void, GetSystemTimeAsFileTime, FILETIME*);
#if _WIN32_WINNT >= 0x0602
__MCF_WINAPI(void, GetSystemTimePreciseAsFileTime, FILETIME*);
#endif
__MCF_WINAPI(ULONGLONG, GetTickCount64, void);
__MCF_WINAPI(BOOL, QueryPerformanceFrequency, LARGE_INTEGER*);
__MCF_WINAPI(BOOL, QueryPerformanceCounter, LARGE_INTEGER*);

typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
typedef DWORD __stdcall THREAD_START_ROUTINE(LPVOID);
__MCF_WINAPI(HANDLE, CreateThread, SECURITY_ATTRIBUTES*, SIZE_T, THREAD_START_ROUTINE*, LPVOID, DWORD, DWORD*);
__MCF_WINAPI(void, ExitThread, DWORD) __attribute__((__noreturn__));
__MCF_WINAPI(BOOL, SwitchToThread, void);
__MCF_WINAPI(BOOL, TerminateProcess, HANDLE, UINT);

typedef BOOL __stdcall HANDLER_ROUTINE(DWORD CtrlType);
__MCF_WINAPI(BOOL, SetConsoleCtrlHandler, HANDLER_ROUTINE*, BOOL);

/* Declare NTDLL (driver) APIs here.  */
__MCF_WINAPI(NTSTATUS, LdrAddRefDll, ULONG, PVOID);
__MCF_WINAPI(BOOLEAN, RtlDllShutdownInProgress, void);

__MCF_WINAPI(void, RtlMoveMemory, void*, const void*, SIZE_T);
__MCF_WINAPI(void, RtlFillMemory, void*, SIZE_T, int);
__MCF_WINAPI(void, RtlZeroMemory, void*, SIZE_T);
__MCF_WINAPI(SIZE_T, RtlCompareMemory, const void*, const void*, SIZE_T) __attribute__((__pure__));

__MCF_WINAPI(NTSTATUS, NtDuplicateObject, HANDLE, HANDLE, HANDLE, HANDLE*, ACCESS_MASK, ULONG, ULONG);
__MCF_WINAPI(NTSTATUS, NtClose, HANDLE);
__MCF_WINAPI(NTSTATUS, NtWaitForSingleObject, HANDLE, BOOLEAN, LARGE_INTEGER*);

__MCF_WINAPI(NTSTATUS, NtWaitForKeyedEvent, HANDLE, PVOID, BOOLEAN, LARGE_INTEGER*);
__MCF_WINAPI(NTSTATUS, NtReleaseKeyedEvent, HANDLE, PVOID, BOOLEAN, LARGE_INTEGER*);

/* Declare helper functions here.  */
__MCF_DECLSPEC_XGLOBALS()
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* __rec, PVOID __estab_frame, CONTEXT* __ctx, PVOID __disp_ctx) __MCF_NOEXCEPT;

#ifdef __i386__

/* SEH is stack-based.  */
typedef struct __MCF_i386_seh_node __MCF_i386_seh_node;

struct __MCF_i386_seh_node
  {
    DWORD __next;
    DWORD __filter;
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

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __MCF_i386_seh_node __MCF_PPCAT2(__MCF_seh_node_, __LINE__)  \
        __attribute__((__cleanup__(__MCF_i386_seh_cleanup)));  \
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
    NTSTATUS __status = NtWaitForKeyedEvent(NULL, (PVOID) __key, 0, (LARGE_INTEGER*) __timeout);
    __MCF_ASSERT(NT_SUCCESS(__status));
    return __status;
  }

__MCF_ALWAYS_INLINE
NTSTATUS
__MCF_keyed_event_signal(const void* __key, const LARGE_INTEGER* __timeout) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtReleaseKeyedEvent(NULL, (PVOID) __key, 0, (LARGE_INTEGER*) __timeout);
    __MCF_ASSERT(NT_SUCCESS(__status));
    return __status;
  }

/* This structure contains timeout values that will be passed to NT syscalls.  */
typedef struct __MCF_winnt_timeout __MCF_winnt_timeout;

struct __MCF_winnt_timeout
  {
    LARGE_INTEGER __li[1];
    uint64_t __since;
  };

__MCF_DECLSPEC_XGLOBALS()
void
__MCF_initialize_winnt_timeout_v2(__MCF_winnt_timeout* __to, const int64_t* __int64_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS()
void
__MCF_adjust_winnt_timeout_v2(__MCF_winnt_timeout* __to) __MCF_NOEXCEPT;

/* Note this function is subject to tail-call optimization.  */
__MCF_DECLSPEC_XGLOBALS()
size_t
__MCF_batch_release_common(const void* __key, size_t __count) __MCF_NOEXCEPT;

/* Copy a block of memory forward, like `memcpy()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mcopy(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mcopy(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT((uintptr_t) __dst - (uintptr_t) __src >= __size);
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __si = (uintptr_t) __src;
    uintptr_t __cx = __size;

    __asm__ (
      __MCF_XASM_BYTES(F3,A4)  // rep movsb

      : "+D"(__di), "+S"(__si), "+c"(__cx), "=m"(*(__bytes*) __dst)
      : "m"(*(const __bytes*) __src)
    );
#else
    /* Call the generic but slower version in NTDLL. We have to call
     * `RtlMoveMemory()` because `RtlCopyMemory()` isn't always exported.  */
    RtlMoveMemory(__dst, __src, __size);
#endif
    return __dst;
  }

/* Copy a block of memory backward.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mcopy_backward(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mcopy_backward(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT((uintptr_t) __src - (uintptr_t) __dst >= __size);
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst + __size - 1;
    uintptr_t __si = (uintptr_t) __src + __size - 1;
    uintptr_t __cx = __size;

    __asm__ (
      __MCF_XASM_BYTES(FD)     // std
      __MCF_XASM_BYTES(F3,A4)  // rep movsb
      __MCF_XASM_BYTES(FC)     // cld

      : "+D"(__di), "+S"(__si), "+c"(__cx), "=m"(*(__bytes*) __dst)
      : "m"(*(const __bytes*) __src)
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlMoveMemory(__dst, __src, __size);
#endif
    return __dst;
  }

/* Fill a block of memory with the given byte, like `memset()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __cx = __size;

    __asm__ (
      __MCF_XASM_BYTES(F3,AA)  // rep stosb

      : "+D"(__di), "+c"(__cx), "=m"(*(__bytes*) __dst)
      : "a"(__val)
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlFillMemory(__dst, __size, __val);
#endif
    return __dst;
  }

/* Fill a block of memory with zeroes, like `bzero()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __cx = __size;

    __asm__ (
      __MCF_XASM_BYTES(F3,AA)  // rep stosb

      : "+D"(__di), "+c"(__cx), "=m"(*(__bytes*) __dst)
      : "a"(0)
    );
#else
    /* Call the generic but slower version in NTDLL.  */
    RtlZeroMemory(__dst, __size);
#endif
    return __dst;
  }

/* Compare two blocks of memory, like `memcmp()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
int
__cdecl
__MCF_mcomp(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
int
__cdecl
__MCF_mcomp(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  {
    int __result;
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __si = (uintptr_t) __src;
    uintptr_t __di = (uintptr_t) __cmp;
    uintptr_t __cx = __size;

    __asm__ (
      __MCF_XASM_BYTES(31,C0)     // xor eax, eax
      __MCF_XASM_BYTES(F3,A6)     // repz cmpsb
      __MCF_XASM_BYTES(0F,95,C0)  // setnz al
      __MCF_XASM_BYTES(19,C9)     // sbb ecx, ecx

      : "=a"(__result), "+S"(__si), "+D"(__di), "+c"(__cx)
      : "m"(*(const __bytes*) __src), "m"(*(const __bytes*) __cmp)
      : "cc"
    );
    __result |= (int) __cx;
#else
    /* Call the generic but slower version in NTDLL.  */
    SIZE_T __n = RtlCompareMemory(__src, __cmp, __size);
    if(__n != __size) {
      __result = *((const uint8_t*)__src + __n) - *((const uint8_t*)__cmp + __n);
      __MCF_ASSERT(__result != 0);
    }
    else
      __result = 0;
#endif
    return __result;
  }

/* Check whether two blocks of memory compare equal, like `memcmp() == 0`.
 * The result is a boolean value.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
bool
__cdecl
__MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
bool
__cdecl
__MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  {
    bool __result;
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __si = (uintptr_t) __src;
    uintptr_t __di = (uintptr_t) __cmp;
    uintptr_t __cx = __size;

    __asm__ (
      __MCF_XASM_BYTES(31,C0)  // xor eax, eax
      __MCF_XASM_BYTES(F3,A6)  // repz cmpsb
#  ifdef __GCC_ASM_FLAG_OUTPUTS__
      : "=@ccz"(__result),
#  else
      __MCF_XASM_BYTES(0F,94,C0)  // setz al
      : "=a"(__result),
#  endif
        "+S"(__si), "+D"(__di), "+c"(__cx)
      : "m"(*(const __bytes*) __src), "m"(*(const __bytes*) __cmp)
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
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __malloc__, __alloc_size__(1)));

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, __size);
    return __ptr;
  }

/* Re-allocate a block of memory, like `realloc()`. If the existent
 * block should be extended, vacuum bytes are filled with zeroes.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__MCF_mrealloc_0(void** __pptr, size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __alloc_size__(2)));

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__MCF_mrealloc_0(void** __pptr, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *__pptr, __size);
    return !__ptr ? NULL : (*__pptr = __ptr);
  }

/* Allocate a copy of a block of memory, like `malloc()` followed by
 * `memcpy()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __alloc_size__(2)));

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void*
__MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapAlloc(GetProcessHeap(), 0, __size);
    return !__ptr ? NULL : __MCF_mcopy(__ptr, __data, __size);
  }

/* Get the size of an allocated block, like `malloc_usable_size()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  {
    size_t __size = HeapSize(GetProcessHeap(), 0, __ptr);
    __MCF_ASSERT(__size != (size_t)-1);
    return __size;
  }

/* Free a block of memory, like `free()`.  */
__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void
__MCF_mfree(void* __ptr) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS(__MCF_GNU_INLINE)
void
__MCF_mfree(void* __ptr) __MCF_NOEXCEPT
  {
    if(!__ptr)
      return;

#ifdef __MCF_DEBUG
    __MCF_mfill(__ptr, 0xFE, HeapSize(GetProcessHeap(), 0, __ptr));
#endif
    int __succ = HeapFree(GetProcessHeap(), 0, __ptr);
    __MCF_ASSERT(__succ);
  }

/* These functions set the last error code and return the second argument.
 * They should be subject to tail-call optimization.  */
__MCF_DECLSPEC_XGLOBALS()
int
__MCF_win32_error_i(DWORD __code, int __val) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS()
void*
__MCF_win32_error_p(DWORD __code, void* __ptr) __MCF_NOEXCEPT;

/* These functions are declared here for the sake of completeness, and are not
 * meant to be called directly.  */
__MCF_DECLSPEC_XGLOBALS()
void
__MCF_run_dtors_at_quick_exit(void) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS()
void
__MCF_run_dtors_atexit(void) __MCF_NOEXCEPT;

__MCF_DECLSPEC_XGLOBALS()
void
__MCF_finalize_on_exit(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_XGLOBALS_  */
