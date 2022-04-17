// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_FWD_H_
#define __MCFGTHREAD_FWD_H_

// Import types about the system ABI.
// Other standard library facilities are not available.
#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
#  error Only native Windows platforms are supported.
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

#ifdef _WIN64
#  define __MCF_PTR_BITS    64
#else
#  define __MCF_PTR_BITS    32
#endif

#ifdef __MCF_DEBUG
#  define __MCF_UNREACHABLE   __builtin_trap()
#else
#  define __MCF_UNREACHABLE   __builtin_unreachable()
#endif

#if defined(__cplusplus)
#  define __MCF_CXX(...)       __VA_ARGS__
#else
#  define __MCF_CXX(...)
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
#  define __MCF_CXX11(...)     __VA_ARGS__
#else
#  define __MCF_CXX11(...)
#endif

#if defined(__cplusplus) && (__cplusplus >= 201402L)
#  define __MCF_CXX14(...)     __VA_ARGS__
#else
#  define __MCF_CXX14(...)
#endif

#define __MCF_GNU_INLINE       extern __inline__ __attribute__((__gnu_inline__))
#define __MCF_NOEXCEPT         __MCF_CXX(throw())
#define __MCF_ALIGNED(...)     __attribute__((__aligned__(__VA_ARGS__)))
#define __MCF_USE_DTOR(...)    __attribute__((__cleanup__(__VA_ARGS__)))

#define __MCFGTHREAD_ASSERT(...)   ((__VA_ARGS__) ? (void) 0 : __MCF_UNREACHABLE)
#define __MCFGTHREAD_CHECK(...)    ((__VA_ARGS__) ? (void) 0 : __builtin_trap())

// Make some forward-declarations.
// Note: Private types need not be declared here.
typedef void* __MCF_HANDLE;
typedef struct __MCF_dtor_queue __MCF_dtor_queue;
typedef struct __MCF_tls_table __MCF_tls_table;

typedef struct __MCF_cond _MCF_cond;
typedef struct __MCF_mutex _MCF_mutex;
typedef struct __MCF_once _MCF_once;
typedef struct __MCF_thread _MCF_thread;
typedef struct __MCF_tls_key _MCF_tls_key;

// Define some helper functions.
__MCF_CXX11(constexpr)
static __inline__ size_t
_MCF_minz(size_t __x, size_t __y) __MCF_NOEXCEPT
  {
    return __y < __x ? __y : __x;
  }

__MCF_CXX11(constexpr)
static __inline__ size_t
_MCF_maxz(size_t __x, size_t __y) __MCF_NOEXCEPT
  {
    return __x < __y ? __y : __x;
  }

// This function is the same as `GetLastError()`.
uint32_t
_MCF_get_win32_error(void) __MCF_NOEXCEPT
  __attribute__((__pure__));

// Performs process cleanup. This function is called by `__cxa_finalize()`. It
// is declared here for the sake of completeness, and is not meant to be called
// directly.
void
__MCF_finalize_on_exit(void) __MCF_NOEXCEPT;

// Declare static data, which are defined in 'startup.c'.
#ifdef __MCFGTHREAD_STARTUP_C_
#  define __MCF_DYNCONST
#else
#  define __MCF_DYNCONST    const   // read-only but initialized dynamically
#endif

extern const __MCF_HANDLE _MCF_crt_module;
extern __MCF_DYNCONST __MCF_HANDLE __MCF_crt_heap;
extern __MCF_DYNCONST uint32_t __MCF_win32_tls_index;
extern __MCF_DYNCONST double __MCF_perf_frequency_reciprocal;
extern __MCF_DYNCONST _MCF_thread __MCF_main_thread;

extern _MCF_mutex __MCF_cxa_atexit_mutex;
extern __MCF_dtor_queue __MCF_cxa_atexit_queue;
extern _MCF_mutex __MCF_cxa_at_quick_exit_mutex;
extern __MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_FWD_H_
