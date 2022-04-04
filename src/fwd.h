// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_FWD_H_
#define __MCFGTHREAD_FWD_H_

// Import types about the system ABI.
// Other standard library facilities are not available.
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <stdbool.h>
#include <stdalign.h>
#include <uchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (0 + __BYTE_ORDER__) != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

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

#define __MCF_PANIC()       __builtin_trap()
#define __MCF_GNU_INLINE    extern __inline__ __attribute__((__gnu_inline__))

// Make some forward-declarations.
typedef struct __MCF_cond _MCF_cond;
typedef struct __MCF_mutex _MCF_mutex;
typedef struct __MCF_once _MCF_once;
typedef struct __MCF_thread_control _MCF_thread_control;

// Declare static data, which are defined in 'startup.c'.
extern void* const _MCF_crt_module;
extern __MCF_DYNCONST uint32_t _MCF_tls_index;
extern __MCF_DYNCONST double _MCF_perf_frequency_reciprocal;
extern __MCF_DYNCONST _MCF_thread_control _MCF_main_thread;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_FWD_H_
