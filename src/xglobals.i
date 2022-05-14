/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_XWINNT_
#define __MCFGTHREAD_XWINNT_

#include "fwd.h"
#include <windows.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_XGLOBALS_EXTERN_INLING
#  define __MCF_XGLOBALS_EXTERN_INLING  __MCF_GNU_INLINE
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
    __MCF_ASSERT(NT_SUCCESS(__status));
    return __status;
  }

__MCF_ALWAYS_INLINE
NTSTATUS
__MCF_keyed_event_signal(const void* __key, const LARGE_INTEGER* __timeout) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtReleaseKeyedEvent(NULL, __key, false, __timeout);
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

void
__MCF_initialize_winnt_timeout_v2(__MCF_winnt_timeout* __to, const int64_t* __int64_opt) __MCF_NOEXCEPT;

void
__MCF_adjust_winnt_timeout_v2(__MCF_winnt_timeout* __to) __MCF_NOEXCEPT;

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

/* Copy a block of memory forward, like `memcpy()`.  */
void* __cdecl
__MCF_mcopy(void* __restrict__ __dst, const void* __restrict__ __src, size_t __size) __MCF_NOEXCEPT;

__MCF_ALWAYS_INLINE __attribute__((__pure__))
bool
__MCF_can_copy_forward(void* __restrict__ __dst, const void* __restrict__ __src, size_t __size) __MCF_NOEXCEPT
  {
    return (uintptr_t) __dst - (uintptr_t) __src >= __size;
  }

__MCF_XGLOBALS_EXTERN_INLING
void* __cdecl
__MCF_mcopy(void* __restrict__ __dst, const void* __restrict__ __src, size_t __size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT(__MCF_can_copy_forward(__dst, __src, __size));
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __si = (uintptr_t) __src;
    uintptr_t __cx = __size;

    __asm__ (
      "rep movsb;"
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

/* Copy a block of potentially overlapped memory, like `memmove()`.  */
void* __cdecl
__MCF_mmove(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

__MCF_XGLOBALS_EXTERN_INLING
void* __cdecl
__MCF_mmove(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __si = (uintptr_t) __src;
    uintptr_t __cx = __size;

    /* If we can't copy forward, adjust pointers and set DF to copy backward.
     * This has to provide a dependency output for the next statement so GCC
     * does not reorder them.  */
    if(!__MCF_can_copy_forward(__dst, __src, __size))
      __asm__ (
        "std;"
        : "=D"(__di), "=S"(__si)
        : "D"(__di + __cx - 1), "S"(__si + __cx - 1)
      );

    /* Now copy memory. The DF flag shall be cleared before returning.  */
    __asm__ (
      "rep movsb;"
      "cld;"
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
void* __cdecl
__MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT;

__MCF_XGLOBALS_EXTERN_INLING
void* __cdecl
__MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __cx = __size;

    __asm__ (
      "rep stosb;"
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
void* __cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT;

__MCF_XGLOBALS_EXTERN_INLING
void* __cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT
  {
#if defined(__i386__) || defined(__amd64__)
    /* Use inline assembly to reduce code size.  */
    typedef char __bytes[__size];
    uintptr_t __di = (uintptr_t) __dst;
    uintptr_t __cx = __size;

    __asm__ (
      "rep stosb;"
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
int __cdecl
__MCF_mcomp(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_XGLOBALS_EXTERN_INLING
int __cdecl
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
      "xorl %%eax, %%eax;"
      "repz cmpsb;"
      "setnzb %%al;"        /* EAX = 0 if equal; 1 if not equal.  */
      "sbbl %%ecx, %%ecx;"  /* ECX = 0 if equal or greater; -1 if less.  */
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
bool __cdecl
__MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_XGLOBALS_EXTERN_INLING
bool __cdecl
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
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT
  __attribute__((__warn_unused_result__, __malloc__, __alloc_size__(1)));

__MCF_XGLOBALS_EXTERN_INLING
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

__MCF_XGLOBALS_EXTERN_INLING
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

__MCF_XGLOBALS_EXTERN_INLING
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

__MCF_XGLOBALS_EXTERN_INLING
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  {
    size_t __size = HeapSize(GetProcessHeap(), 0, __ptr);
    __MCF_ASSERT(__size != (size_t)-1);
    return __size;
  }

/* Free a block of memory, like `free()`.  */
void
__MCF_mfree(void* __ptr) __MCF_NOEXCEPT;

__MCF_XGLOBALS_EXTERN_INLING
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
int
__MCF_win32_error_i(DWORD __code, int __val) __MCF_NOEXCEPT;

void*
__MCF_win32_error_p(DWORD __code, void* __ptr) __MCF_NOEXCEPT;

/* These functions are declared here for the sake of completeness, and are not
 * meant to be called directly.  */
void
__MCF_run_dtors_at_quick_exit(void) __MCF_NOEXCEPT;

void
__MCF_run_dtors_atexit(void) __MCF_NOEXCEPT;

void
__MCF_finalize_on_exit(void) __MCF_NOEXCEPT;

/* Declare global data.  */
#ifndef __MCF_DYNCONST
#  define __MCF_DYNCONST   const  /* read-only but initialized dynamically  */
#endif

extern __MCF_DYNCONST DWORD __MCF_win32_tls_index;
extern __MCF_DYNCONST double __MCF_perf_frequency_reciprocal;
extern __MCF_DYNCONST _MCF_thread __MCF_main_thread;

extern _MCF_mutex __MCF_cxa_atexit_mutex;
extern __MCF_dtor_queue __MCF_cxa_atexit_queue;
extern _MCF_mutex __MCF_cxa_at_quick_exit_mutex;
extern __MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;
extern BYTE __MCF_mutex_spin_field[2048];

/* These are private functions that are not exported from the DLL.  */
void
__MCF_dll_callback_on_process_attach(void) __MCF_NOEXCEPT;

void
__MCF_dll_callback_on_thread_detach(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_XGLOBALS_  */
