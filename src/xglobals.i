/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_XGLOBALS_
#define __MCFGTHREAD_XGLOBALS_

#include "fwd.h"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "dtor_queue.h"
#include <minwindef.h>
#include <ntdef.h>
#include <winerror.h>
#include <ntstatus.h>

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_XGLOBALS_IMPORT
#  define __MCF_XGLOBALS_IMPORT
#  define __MCF_XGLOBALS_INLINE  __MCF_GNU_INLINE
#  define __MCF_XGLOBALS_READONLY   const
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

/* Undefine macros that redirect to standard functions.
 * This ensures we call the ones from KERNEL32.  */
#undef RtlCopyMemory
#undef RtlMoveMemory
#undef RtlFillMemory
#undef RtlZeroMemory
#undef RtlCompareMemory
#undef RtlEqualMemory

/* Hard-code these.  */
#define GetCurrentProcess()  ((HANDLE) -1)
#define GetCurrentThread()   ((HANDLE) -2)

#define __MCF_ASSERT_NT(...)  __MCF_ASSERT(NT_SUCCESS(__VA_ARGS__))
#define __MCF_CHECK_NT(...)   __MCF_CHECK(NT_SUCCESS(__VA_ARGS__))

#define __MCF_WINAPI(RETURN, function, ...)  \
  RETURN __stdcall function(__VA_ARGS__) __attribute__((__dllimport__, __nothrow__))

/* Declare KERNEL32 APIs here.  */
__MCF_WINAPI(DWORD, GetLastError, void) __attribute__((__pure__));
__MCF_WINAPI(void, SetLastError, DWORD);

__MCF_WINAPI(PVOID, EncodePointer, PVOID) __attribute__((__const__));
__MCF_WINAPI(PVOID, DecodePointer, PVOID) __attribute__((__const__));

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
__MCF_WINAPI(int, GetThreadPriority, HANDLE) __attribute__((__pure__));
__MCF_WINAPI(BOOL, SetThreadPriority, HANDLE, int);
__MCF_WINAPI(DWORD, GetCurrentProcessId, void);
__MCF_WINAPI(BOOL, TerminateProcess, HANDLE, UINT);

typedef BOOL __stdcall HANDLER_ROUTINE(DWORD);
__MCF_WINAPI(BOOL, SetConsoleCtrlHandler, HANDLER_ROUTINE*, BOOL);

__MCF_WINAPI(NTSTATUS, BaseGetNamedObjectDirectory, HANDLE*);

/* Declare NTDLL (driver) APIs here.  */
__MCF_WINAPI(NTSTATUS, LdrAddRefDll, ULONG, PVOID);
__MCF_WINAPI(BOOLEAN, RtlDllShutdownInProgress, void);

__MCF_WINAPI(void, RtlMoveMemory, void*, const void*, SIZE_T);
__MCF_WINAPI(void, RtlFillMemory, void*, SIZE_T, int);
__MCF_WINAPI(void, RtlZeroMemory, void*, SIZE_T);
__MCF_WINAPI(SIZE_T, RtlCompareMemory, const void*, const void*, SIZE_T) __attribute__((__pure__));

__MCF_WINAPI(NTSTATUS, NtCreateSection, HANDLE*, ACCESS_MASK, OBJECT_ATTRIBUTES*, LARGE_INTEGER*, ULONG, ULONG, HANDLE);
__MCF_WINAPI(NTSTATUS, NtMapViewOfSection, HANDLE, HANDLE, PVOID*, ULONG_PTR, SIZE_T, LARGE_INTEGER*, SIZE_T*, ULONG, ULONG, ULONG);
__MCF_WINAPI(NTSTATUS, NtUnmapViewOfSection, HANDLE, PVOID);
__MCF_WINAPI(NTSTATUS, NtProtectVirtualMemory, HANDLE, PVOID*, SIZE_T*, ULONG, ULONG*);

__MCF_WINAPI(NTSTATUS, NtDuplicateObject, HANDLE, HANDLE, HANDLE, HANDLE*, ACCESS_MASK, ULONG, ULONG);
__MCF_WINAPI(NTSTATUS, NtClose, HANDLE);
__MCF_WINAPI(NTSTATUS, NtWaitForSingleObject, HANDLE, BOOLEAN, LARGE_INTEGER*);
__MCF_WINAPI(NTSTATUS, NtDelayExecution, BOOLEAN, LARGE_INTEGER*);

__MCF_WINAPI(NTSTATUS, NtWaitForKeyedEvent, HANDLE, PVOID, BOOLEAN, LARGE_INTEGER*);
__MCF_WINAPI(NTSTATUS, NtReleaseKeyedEvent, HANDLE, PVOID, BOOLEAN, LARGE_INTEGER*);

/* Declare helper functions here.  */
__MCF_XGLOBALS_IMPORT
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* __rec, PVOID __estab_frame, CONTEXT* __ctx, PVOID __disp_ctx) __MCF_NOEXCEPT;

#if defined(__i386__)
/* On x86, SEH is stack-based.  */
typedef struct __MCF_seh_i386_node __MCF_seh_i386_node;

struct __MCF_seh_i386_node
  {
    DWORD __next;
    DWORD __filter;
  };

__MCF_ALWAYS_INLINE
__MCF_seh_i386_node*
__MCF_seh_i386_install(__MCF_seh_i386_node* __seh_node) __MCF_NOEXCEPT
  {
    __asm__ volatile ("{ movl %%fs:0, %0 | mov %0, fs:[0] }" : "=r"(__seh_node->__next));
    __seh_node->__filter = (DWORD) __MCF_seh_top;
    __asm__ volatile ("{ movl %0, %%fs:0 | mov fs:[0], %0 }" : : "r"(__seh_node));
    return __seh_node;
  }

__MCF_ALWAYS_INLINE
void
__MCF_seh_i386_cleanup(__MCF_seh_i386_node* __seh_node) __MCF_NOEXCEPT
  {
    __asm__ volatile ("{ movl %0, %%fs:0 | mov fs:[0], %0 }" : : "r"(__seh_node->__next));
  }

#  define __MCF_SEH_I386_NODE_NY(n)  __MCF_seh_i386_node_##n
#  define __MCF_SEH_I386_NODE_NX(n)  __MCF_SEH_I386_NODE_NY(n)
#  define __MCF_SEH_I386_NODE_NAME_HERE   __MCF_SEH_I386_NODE_NX(__LINE__)

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __MCF_seh_i386_node __MCF_SEH_I386_NODE_NAME_HERE  \
        __attribute__((__cleanup__(__MCF_seh_i386_cleanup)))  \
        = *__MCF_seh_i386_install(&__MCF_SEH_I386_NODE_NAME_HERE)  /* no semicolon  */
#else
/* Otherwise, SEH is table-based.  */
#  ifdef __arm__
#    define __MCF_SEH_FLAG_PREFIX   "%"
#  else
#    define __MCF_SEH_FLAG_PREFIX   "@"
#  endif

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __asm__ volatile (".seh_handler "  \
        " __MCF_seh_top, " __MCF_SEH_FLAG_PREFIX "except")  /* no semicolon  */
#endif

/* This structure contains timeout values that will be passed to NT syscalls.  */
typedef struct __MCF_winnt_timeout __MCF_winnt_timeout;

struct __MCF_winnt_timeout
  {
    LARGE_INTEGER __li[1];
    uint64_t __since;
  };

__MCF_XGLOBALS_IMPORT
void
__MCF_initialize_winnt_timeout_v2(__MCF_winnt_timeout* __to, const int64_t* __int64_opt) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void
__MCF_adjust_winnt_timeout_v2(__MCF_winnt_timeout* __to) __MCF_NOEXCEPT;

/* Note this function is subject to tail-call optimization.  */
__MCF_XGLOBALS_IMPORT
size_t
__MCF_batch_release_common(const void* __key, size_t __count) __MCF_NOEXCEPT;

/* Copy a block of memory forward, like `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

/* Copy a block of memory backward.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy_backward(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

/* Fill a block of memory with the given byte, like `memset()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT;

/* Fill a block of memory with zeroes, like `bzero()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT;

/* Allocate a block of zeroed memory, like `calloc()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT __attribute__((__warn_unused_result__, __malloc__, __alloc_size__(1)));

/* Re-allocate a block of memory, like `realloc()`. If the existent
 * block should be extended, vacuum bytes are filled with zeroes.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mrealloc_0(void** __pptr, size_t __size) __MCF_NOEXCEPT __attribute__((__warn_unused_result__, __alloc_size__(2)));

/* Allocate a copy of a block of memory, like `malloc()` followed by
 * `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT __attribute__((__warn_unused_result__, __alloc_size__(2)));

/* Get the size of an allocated block, like `malloc_usable_size()`.  */
__MCF_XGLOBALS_INLINE
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT __attribute__((__pure__));

/* Free a block of memory, like `free()`.  */
__MCF_XGLOBALS_INLINE
void
__MCF_mfree(void* __ptr_opt) __MCF_NOEXCEPT;

/* These functions set the last error code and return the second argument.
 * They should be subject to tail-call optimization.  */
__MCF_XGLOBALS_IMPORT
int
__MCF_win32_error_i(DWORD __code, int __val) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void*
__MCF_win32_error_p(DWORD __code, void* __ptr) __MCF_NOEXCEPT;

/* These functions are declared here for the sake of completeness, and are not
 * meant to be called directly.  */
__MCF_XGLOBALS_IMPORT
void
__MCF_run_dtors_at_quick_exit(void) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void
__MCF_run_dtors_atexit(void) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void
__MCF_finalize_on_exit(void) __MCF_NOEXCEPT;

/* Declare global data.  */
typedef struct __MCF_crt_xglobals __MCF_crt_xglobals;

struct __MCF_crt_xglobals
  {
    __MCF_crt_xglobals* __self_ptr;
    DWORD __self_size;

    /* global resources  */
    DWORD __tls_index;
    _MCF_thread __main_thread[1];

    /* `atexit()` support  */
    _MCF_mutex __cxa_atexit_mutex[1];
    __MCF_dtor_queue __cxa_atexit_queue[1];

    /* `at_quick_exit()` support  */
    _MCF_mutex __cxa_at_quick_exit_mutex[1];
    __MCF_dtor_queue __cxa_at_quick_exit_queue[1];

    /* mutex support  */
    BYTE __mutex_spin_field[2048] __MCF_ALIGN(64);

    /* thread suspension support  */
    _MCF_cond __interrupt_cond[1];
  };

/* These are constants that have to be initialized at load time.  */
extern HANDLE __MCF_XGLOBALS_READONLY __MCF_crt_heap;
extern double __MCF_XGLOBALS_READONLY __MCF_crt_pf_recip;

/* This is a pointer to the process-specific data.
 * As mcfgthread may be linked statically by user DLLs, we must ensure that, in
 * the same process, all instances of this pointer point to the same object. This
 * is achieved by having them point to a named file mapping, which is created
 * by the current process with exclusive access, and whose name is generated from
 * its process ID. Additional randomness is introduced to prevent the name from
 * being predicted.  */
extern __MCF_crt_xglobals* __MCF_XGLOBALS_READONLY __MCF_g;

/* As `__MCF_crt_xglobals` is shared between all static and shared instances of
 * this library within a single process, we have to involve sort of versioning.  */
#define __MCF_G_FIELD_OPT(field)  \
    (((unsigned long) __builtin_expect((long) __MCF_g->__self_size, sizeof(__MCF_crt_xglobals))  \
           >= __builtin_offsetof(__MCF_crt_xglobals, field) + sizeof(__MCF_g->field))  \
        ? &(__MCF_g->field) : NULL)

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_ALWAYS_INLINE
NTSTATUS
__MCF_keyed_event_wait(const void* __key, const LARGE_INTEGER* __timeout) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtWaitForKeyedEvent(NULL, (PVOID) __key, 0, (LARGE_INTEGER*) __timeout);
    __MCF_ASSERT_NT(__status);
    return __status;
  }

__MCF_ALWAYS_INLINE
NTSTATUS
__MCF_keyed_event_signal(const void* __key, const LARGE_INTEGER* __timeout) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtReleaseKeyedEvent(NULL, (PVOID) __key, 0, (LARGE_INTEGER*) __timeout);
    __MCF_ASSERT_NT(__status);
    return __status;
  }

__MCF_ALWAYS_INLINE
void
__MCF_close_handle(__MCF_HANDLE __handle) __MCF_NOEXCEPT
  {
    NTSTATUS __status = NtClose(__handle);
    __MCF_ASSERT_NT(__status);
  }

#if defined(__i386__) || defined(__amd64__)
/* Define macros for string operations for reducing code size.  */
#  define __MCF_X86_REP_STOSB(di, cx, ax)  \
    __asm__ volatile (  \
        "rep stosb"  \
          : "+D"(*(di)), "+c"(*(cx)) : "a"(ax)  \
          : "memory")  /* no semicolon  */

#  define __MCF_X86_REP_MOVSB(di, si, cx)  \
    __asm__ volatile (  \
        "rep movsb"  \
          : "+D"(*(di)), "+S"(*(si)), "+c"(*(cx)) :  \
          : "memory")  /* no semicolon  */

#  define __MCF_X86_REP_MOVSB_R(di, si, cx)  \
    __asm__ volatile (  \
        "std; rep movsb; cld"  \
          : "+D"(*(di)), "+S"(*(si)), "+c"(*(cx)) :  \
          : "memory")  /* no semicolon  */
#endif

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT((uintptr_t) __dst - (uintptr_t) __src >= __size);
#ifdef __MCF_X86_REP_MOVSB
    char* __di = (char*) __dst;
    char* __si = (char*) __src;
    size_t __cx = __size;
    __MCF_X86_REP_MOVSB(&__di, &__si, &__cx);
#else
    RtlMoveMemory(__dst, __src, __size);
#endif
    return __dst;
  }

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy_backward(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT((uintptr_t) __src - (uintptr_t) __dst >= __size);
#ifdef __MCF_X86_REP_MOVSB_R
    char* __di = (char*) __dst + __size - 1;
    char* __si = (char*) __src + __size - 1;
    size_t __cx = __size;
    __MCF_X86_REP_MOVSB_R(&__di, &__si, &__cx);
#else
    RtlMoveMemory(__dst, __src, __size);
#endif
    return __dst;
  }

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT
  {
#ifdef __MCF_X86_REP_STOSB
    char* __di = (char*) __dst;
    size_t __cx = __size;
    __MCF_X86_REP_STOSB(&__di, &__cx, __val);
#else
    RtlFillMemory(__dst, __size, __val);
#endif
    return __dst;
  }

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT
  {
#ifdef __MCF_X86_REP_STOSB
    char* __di = (char*) __dst;
    size_t __cx = __size;
    __MCF_X86_REP_STOSB(&__di, &__cx, 0);
#else
    RtlZeroMemory(__dst, __size);
#endif
    return __dst;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_0(size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, __size);
    return __ptr;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mrealloc_0(void** __pptr, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, *__pptr, __size);
    return !__ptr ? NULL : (*__pptr = __ptr);
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr = HeapAlloc(__MCF_crt_heap, 0, __size);
    return !__ptr ? NULL : __MCF_mcopy(__ptr, __data, __size);
  }

__MCF_XGLOBALS_INLINE
size_t
__MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  {
    size_t __size = HeapSize(__MCF_crt_heap, 0, __ptr);
    __MCF_ASSERT(__size != (size_t)-1);
    return __size;
  }

__MCF_XGLOBALS_INLINE
void
__MCF_mfree(void* __ptr_opt) __MCF_NOEXCEPT
  {
    if(!__ptr_opt)
      return;

#ifdef __MCF_DEBUG
    __MCF_mfill(__ptr_opt, 0xFE, HeapSize(__MCF_crt_heap, 0, __ptr_opt));
#endif
    int __succ = HeapFree(__MCF_crt_heap, 0, __ptr_opt);
    __MCF_ASSERT(__succ);
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_XGLOBALS_  */
