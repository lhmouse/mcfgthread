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

// Informs the runtime that threading support is active.
// Windows creates new threads for console control handlers, so threading
// cannot be disabled.
int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  __attribute__((__const__));

#define __gthread_active_p  __MCF_gthr_active_p

__MCF_CXX11(constexpr)
__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  {
    return 1;
  }

// This is an auxiliary function for exception handling in `__gthread_once()`.
// Ideally, if the target function throws exception we would like to allow
// attempts to retry. Sadly this is not possible in standard C.
void
__MCF_gthr_unonce(__gthread_once_t** __oncep) __MCF_NOEXCEPT;

__MCFGTHREAD_GTHR_INLINE void
__MCF_gthr_unonce(__gthread_once_t** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

// Performs one-time initialization, like `pthread_once()`.
int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void));

#define __gthread_once  __MCF_gthr_once

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void))
  {
    int __do_init = _MCF_once_wait(__once, NULL);
    if(__do_init == 0)
      return 0;

    _MCF_once* __cl __attribute__((__cleanup__(__MCF_gthr_unonce)));
    __cl = __once;
    __init_proc();
    __cl = NULL;
    _MCF_once_release(__once);
    return 0;
  }

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_GTHR_H_
