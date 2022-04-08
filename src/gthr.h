// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_GTHR_H_
#define __MCFGTHREAD_GTHR_H_

#include "fwd.h"
#include "tls.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include <time.h>  // struct timespec
#include <errno.h>  // E* macros

#ifdef __cplusplus
extern "C" {
#endif

// Define wrappers as required by 'gthr-default.h'.
#ifndef __MCFGTHREAD_GTHR_C_
#  define __MCFGTHREAD_GTHR_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_GTHR_INLINE
#endif

// Enable full C++11 threading support.
#define __GTHREADS  1
#define __GTHREADS_CXX0X  1
#define __GTHREAD_HAS_COND  1

// Define gthread types. These usually map to our APIs directly, except
// the recursive mutex.
typedef struct timespec __gthread_time_t;
typedef _MCF_thread* __gthread_t;
typedef _MCF_tls_key* __gthread_key_t;

typedef _MCF_once __gthread_once_t;
typedef _MCF_cond __gthread_cond_t;
typedef _MCF_mutex __gthread_mutex_t;

struct __MCF_gthr_recursive_mutex_t
  {
    uint32_t __owner;  // owner thread ID
    int32_t __depth;  // recursion depth
    _MCF_mutex __mutex;
  }
  typedef __gthread_recursive_mutex_t;

// Define macros for static and dynamic initialization.
#define __GTHREAD_ONCE_INIT  {0}
#define __GTHREAD_COND_INIT  {0}
#define __GTHREAD_MUTEX_INIT  {0}
#define __GTHREAD_RECURSIVE_MUTEX_INIT  {0}

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_GTHR_H_
