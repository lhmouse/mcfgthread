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

#ifndef __MCF_STARTUP
#  define __MCF_DYNCONST  const   // read-only but initialized dynamically
#else
#  define __MCF_DYNCONST
#endif

// Make some forward-declarations.
typedef struct _MCF_thread_control _MCF_thread_control;
typedef void _MCF_thread_procedure(_MCF_thread_control* __control);

typedef struct _MCF_mutex _MCF_mutex;
typedef struct _MCF_cond _MCF_cond;
typedef struct _MCF_once _MCF_once;

// Declare static data, which are defined in 'startup.c'.
extern void* const _MCF_crt_module;
extern __MCF_DYNCONST uint32_t _MCF_tls_index;
extern __MCF_DYNCONST _MCF_thread_control _MCF_main_thread;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_FWD_H_
