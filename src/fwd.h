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
typedef struct __MCF_thread_control __MCF_thread_control;
typedef void __MCF_thread_procedure(__MCF_thread_control* __control);

typedef struct __MCF_mutex __MCF_mutex;
typedef struct __MCF_cond __MCF_cond;
typedef struct __MCF_once __MCF_once;

// Declare static data, which are defined in 'startup.c'.
extern void* const __MCF_crt_module;
extern __MCF_DYNCONST uint32_t __MCF_tls_index;
extern __MCF_DYNCONST __MCF_thread_control __MCF_main_thread;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_FWD_H_
