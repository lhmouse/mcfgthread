/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_FWD_
#define __MCFGTHREAD_FWD_

/* Import types about the system ABI.
 * Other standard library facilities are not available.  */
#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <limits.h>

#ifdef __SSE2__
#  include <emmintrin.h>
#endif

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

#if defined(__cplusplus)
#  define __MCF_C(...)
#else
#  define __MCF_C(...)   __VA_ARGS__
#endif

#if defined(__cplusplus)
#  define __MCF_CXX(...)   __VA_ARGS__
#else
#  define __MCF_CXX(...)
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
#  define __MCF_CXX11(...)   __VA_ARGS__
#else
#  define __MCF_CXX11(...)
#endif

#if defined(__cplusplus) && (__cplusplus >= 201402L)
#  define __MCF_CXX14(...)   __VA_ARGS__
#else
#  define __MCF_CXX14(...)
#endif

#define __MCF_PPCAT2(x,y)     x##y
#define __MCF_PPCAT3(x,y,z)   x##y##z
#define __MCF_PPLAZY(f,...)   f(__VA_ARGS__)

#define __MCF_GNU_INLINE      extern __inline__ __attribute__((__gnu_inline__))
#define __MCF_ALWAYS_INLINE   __MCF_GNU_INLINE __attribute__((__always_inline__, __artificial__))
#define __MCF_NEVER_INLINE    __attribute__((__noinline__))
#define __MCF_NOEXCEPT        __MCF_CXX(throw())
#define __MCF_ALIGNED(...)    __attribute__((__aligned__(__VA_ARGS__)))
#define __MCF_USE_DTOR(...)   __attribute__((__cleanup__(__VA_ARGS__)))
#define __MCF_0_INIT          { __MCF_C(0) }
#define __MCF_PTR_BITS        (__SIZEOF_POINTER__ * 8U)

#ifndef __MCF_DECLSPEC_FWD
#  define __MCF_DECLSPEC_FWD(...)  __VA_ARGS__
#endif

/* The `__MCF_STATIC_ASSERT()` macro is an expression that yields zero if it
 * compiles anyway. Its argument must be a constant expression.  */
#ifdef __cplusplus
extern "C++" template<bool __value> struct __MCF_static_assert;
extern "C++" template<> struct __MCF_static_assert<true> { char __unused;  };
#  define __MCF_STATIC_ASSERT(...)   ((int) sizeof(__MCF_static_assert<(__VA_ARGS__)>) - 1)
#else
#  define __MCF_STATIC_ASSERT(...)   ((int) sizeof(struct { char: 1|-!(__VA_ARGS__); }) - 1)
#endif

#ifdef __MCF_DEBUG
#  define __MCF_UNREACHABLE   __MCF_runtime_failure(__func__)
#else
#  define __MCF_UNREACHABLE   __builtin_unreachable()
#endif

/* The `__MCF_ASSERT()` and `__MCF_CHECK()` macros perform run-time checks. If
 * an argument yields false, `__MCF_ASSERT()` results in undefined behavior,
 * and `__MCF_CHECK()` effects abnormal termination of the current program.  */
#define __MCF_ASSERT(...)     ((__VA_ARGS__) ? (void) 0 : __MCF_UNREACHABLE)
#define __MCF_CHECK(...)      ((__VA_ARGS__) ? (void) 0 : __MCF_runtime_failure(__func__))

/* Make some forward declarations.  */
typedef struct __MCF_dtor_element __MCF_dtor_element;
typedef struct __MCF_dtor_queue __MCF_dtor_queue;
typedef struct __MCF_tls_table __MCF_tls_table;
typedef struct __MCF_tls_element __MCF_tls_element;
typedef union __MCF_cxa_dtor_union __MCF_cxa_dtor_union;

typedef struct __MCF_cond _MCF_cond;
typedef struct __MCF_mutex _MCF_mutex;
typedef struct __MCF_once _MCF_once;
typedef struct __MCF_sem _MCF_sem;
typedef struct __MCF_event _MCF_event;
typedef struct __MCF_thread _MCF_thread;
typedef struct __MCF_tls_key _MCF_tls_key;

typedef void* __MCF_HANDLE;

/* See `_MCF_cond_wait()` for details about these callbacks.  */
typedef intptr_t _MCF_cond_unlock_callback(intptr_t __lock_arg);
typedef void _MCF_cond_relock_callback(intptr_t __lock_arg, intptr_t __unlocked);

/* Define the prototype for thread procedures.  */
typedef void _MCF_thread_procedure(_MCF_thread* __thrd);

/* Define the prototype for destructors for `_MCF_tls_key_new()`.  */
typedef void _MCF_tls_dtor(void* __ptr);

/* Note: In the case of i386, the argument is passed both via the ECX register
 * and on the stack, to allow both `__cdecl` and `__thiscall` functions to work
 * properly. The function prototype is declared for compatibility with GCC.  */
typedef void __cdecl __MCF_cxa_dtor_cdecl(void* __this);
typedef void __thiscall __MCF_cxa_dtor_thiscall(void* __this);

/* Define the prototype for `atexit()` and `at_quick_exit()`.  */
typedef void __MCF_atexit_callback(void);

/* Define some helper functions.  */
__MCF_CXX11(constexpr) __MCF_ALWAYS_INLINE
size_t
_MCF_minz(size_t __x, size_t __y) __MCF_NOEXCEPT
  {
    return (__y < __x) ? __y : __x;
  }

__MCF_CXX11(constexpr) __MCF_ALWAYS_INLINE
size_t
_MCF_maxz(size_t __x, size_t __y) __MCF_NOEXCEPT
  {
    return (__x < __y) ? __y : __x;
  }

__MCF_DECLSPEC_FWD()
void
__MCF_runtime_failure(const char* __where)
  __attribute__((__noreturn__, __noinline__, __cold__));

__MCF_DECLSPEC_FWD()
uint32_t
_MCF_get_win32_error(void) __MCF_NOEXCEPT
  __attribute__((__pure__));

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_FWD_  */
