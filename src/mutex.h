/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_MUTEX_H_
#define __MCFGTHREAD_MUTEX_H_

#include "fwd.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_MUTEX_EXTERN_INLINE
#  define __MCF_MUTEX_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* Define the mutex struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_mutex
  {
    uintptr_t __locked : 1;

    uintptr_t __sp_nthrd : 4;  /* number of spinning threads  */
#define __MCF_MUTEX_SP_NTHRD  1U

    uintptr_t __sp_nfail : 4;  /* number of timeouts after spinning  */
#define __MCF_MUTEX_SP_NFAIL_M  15U

    uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
#define __MCF_MUTEX_NSLEEP_M  (UINTPTR_MAX >> 9)
  }
  typedef _MCF_mutex;

/* If the spinning failure counter has reached this number, newcomers will not
 * attempt to spin at all.
 * This value must not be greater than `__MCF_MUTEX_SP_NFAIL_M`, and must not
 * be zero.  */
#define __MCF_MUTEX_SPIN_FAIL_THRESHOLD   10U

/* This is the initial number of iterations that a thread may spin before it
 * goes to sleep. As spinning starts to fail more frequently, later threads
 * spin fewer times, until the number drops to zero.
 * This value had better be divisible by `__MCF_MUTEX_SPIN_FAIL_THRESHOLD`.  */
#define __MCF_MUTEX_MAX_SPIN_COUNT      1280U

/* Initializes a mutex dynamically.
 * Static ones should be initialized with `{0}`, like other structs.  */
void
_MCF_mutex_init(_MCF_mutex* __mutex) __MCF_NOEXCEPT;

__MCF_MUTEX_EXTERN_INLINE void
_MCF_mutex_init(_MCF_mutex* __mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex __temp = { 0 };
    __MCF_ATOMIC_STORE_PTR_REL(__mutex, &__temp);
  }

/* Attempts to lock a mutex.
 * This a simple mutex that is not recursive and performs no error checking. If
 * the caller attempts to lock a mutex which it has already held, deadlocks may
 * occur.
 *
 * If the `timeout` argument points to a positive integer, it denotes the wait
 * expiration time, in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the mutex has been locked by the caller, or -1 if the operation
 * has timed out.  */
int
_MCF_mutex_lock(_MCF_mutex* __mutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

int
_MCF_mutex_lock_slow(_MCF_mutex* __mutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCF_MUTEX_EXTERN_INLINE int
_MCF_mutex_lock(_MCF_mutex* __mutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
    _MCF_mutex __old;
    __MCF_ATOMIC_LOAD_PTR_RLX(&__old, __mutex);

    if(__timeout_opt && (*__timeout_opt == 0) && __builtin_expect(__old.__locked, 0))
      return -1;

    return _MCF_mutex_lock_slow(__mutex, __timeout_opt);
  }

/* Releases a mutex. This function may be called by a different thread from
 * which locked the same mutex. If the mutex has not been locked, the behavior
 * is undefined.  */
void
_MCF_mutex_unlock(_MCF_mutex* __mutex) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_MUTEX_H_  */
