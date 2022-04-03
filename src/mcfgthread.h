// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCF_MCFGTHREAD_H_
#define __MCF_MCFGTHREAD_H_

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

#ifndef __MCF_STARTUP

// User code sees these.
#  define __MCF_DYNCONST  const   // read-only data, but initialized dynamically
#  define __MCF_INLINE  extern __inline__ __attribute__((__gnu_inline__))   // `inline` without a definition

#else  // __MCF_STARTUP

// The library itself sees these.
#  define __MCF_DYNCONST
#  define __MCF_INLINE  extern

#endif  // __MCF_STARTUP

// Define the thread information struct.
struct __MCF_mopthread
  {
    int __nref[1];   // atomic reference count
    uint32_t __tid;  // thread id
    void* __handle;  // win32 thread handle
    void* __status;  // exit code
    void* __param;   // startup parameter
  }
  typedef __MCF_mopthread_t;

// Define static data.
extern void* const __MCF_crt_module;
extern __MCF_DYNCONST uint32_t __MCF_tls_index;
extern __MCF_DYNCONST __MCF_mopthread_t __MCF_main_thread;

#ifdef __cplusplus
}
#endif

#endif  // __MCF_MCFGTHREAD_H_
