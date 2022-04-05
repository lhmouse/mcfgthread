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

#ifdef __cplusplus
extern "C" {
#endif

#if (0 + __BYTE_ORDER__) != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

#define __MCF_GNU_INLINE       extern __inline__ __attribute__((__gnu_inline__))
#define __MCFGTHREAD_CHECK(...)   ((__VA_ARGS__) ? (void) 0 : __builtin_trap())

#ifdef __cplusplus
#  define __MCF_NOEXCEPT    throw()
#else
#  define __MCF_NOEXCEPT
#endif

#ifndef __MCFGTHREAD_STARTUP_C_
#  define __MCF_DYNCONST    const   // read-only but initialized dynamically
#else
#  define __MCF_DYNCONST
#endif

#ifdef __MCF_DEBUG
#  define __MCFGTHREAD_ASSERT(...)  __MCFGTHREAD_CHECK(__VA_ARGS__))
#else
#  define __MCFGTHREAD_ASSERT(...)  ((__VA_ARGS__) ? (void) 0 : __builtin_unreachable())
#endif

// Make some forward-declarations.
typedef struct __MCF_cond _MCF_cond;
typedef struct __MCF_mutex _MCF_mutex;
typedef struct __MCF_once _MCF_once;
typedef struct __MCF_thread _MCF_thread;

typedef void _MCF_thread_procedure(_MCF_thread* __thrd);
typedef void _MCF_tls_destructor(void* __data);

// Define some helper functions.
static __inline__ size_t
_MCF_minz(size_t __x, size_t __y) __MCF_NOEXCEPT
  { return __y < __x ? __y : __x;  }

// Declare static data, which are defined in 'startup.c'.
extern void* const _MCF_crt_module;
extern __MCF_DYNCONST uint32_t _MCF_tls_index;
extern __MCF_DYNCONST double _MCF_perf_frequency_reciprocal;
extern __MCF_DYNCONST _MCF_thread _MCF_main_thread;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_FWD_H_
