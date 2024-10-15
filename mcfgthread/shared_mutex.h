/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_SHARED_MUTEX_
#define __MCFGTHREAD_SHARED_MUTEX_

#include "fwd.h"
#include "atomic.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_SHARED_MUTEX_IMPORT
#  define __MCF_SHARED_MUTEX_IMPORT
#  define __MCF_SHARED_MUTEX_INLINE  __MCF_GNU_INLINE
#endif

/* Define the shared mutex struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_shared_mutex
  {
    __MCF_EX uintptr_t __nshare : 14;  /* number of sharing threads  */
    __MCF_EX uintptr_t __nsleep : __MCF_PTR_BITS - 14;  /* number of sleeping threads  */
  };

/* This is the maximum number of concurrent threads with shared access. If
 * `__nshare` exceeds this value, further threads will block. This value meets
 * the requirement of the C++ standard, which is 10000.  */
#define __MCF_SHARED_MUTEX_MAX_SHARE   16382U

/* Initializes a shared mutex dynamically. Static ones should be initialized with
 * `{0}`, like other structs.  */
__MCF_SHARED_MUTEX_INLINE
void
_MCF_shared_mutex_init(_MCF_shared_mutex* __smutex) __MCF_NOEXCEPT;

/* Attempts to lock a shared mutex in shared mode.
 *
 * A shared mutex is not recursive and performs no error checking. If the caller
 * attempts to lock a mutex which it has already held and there is another thread
 * waiting for exclusive access, deadlocks may occur.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the shared mutex has been locked in shared mode by the caller, or
 * -1 if the operation has timed out.  */
__MCF_SHARED_MUTEX_IMPORT
int
_MCF_shared_mutex_lock_shared_slow(_MCF_shared_mutex* __smutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCF_SHARED_MUTEX_INLINE
int
_MCF_shared_mutex_lock_shared(_MCF_shared_mutex* __smutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Attempts to lock a shared mutex in exclusive mode.
 *
 * A shared mutex is not recursive and performs no error checking. If the caller
 * attempts to lock a mutex which it has already held, deadlocks may occur.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the shared mutex has been locked in exclusive mode by the caller,
 * or -1 if the operation has timed out.  */
__MCF_SHARED_MUTEX_IMPORT
int
_MCF_shared_mutex_lock_exclusive_slow(_MCF_shared_mutex* __smutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCF_SHARED_MUTEX_INLINE
int
_MCF_shared_mutex_lock_exclusive(_MCF_shared_mutex* __smutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Releases a shared mutex, in either shared or exclusive mode. This function may
 * be called by a different thread from which locked the same mutex. If the mutex
 * has not been locked, the behavior is undefined.  */
__MCF_SHARED_MUTEX_IMPORT
void
_MCF_shared_mutex_unlock_slow(_MCF_shared_mutex* __smutex) __MCF_NOEXCEPT;

__MCF_SHARED_MUTEX_INLINE
void
_MCF_shared_mutex_unlock(_MCF_shared_mutex* __smutex) __MCF_NOEXCEPT;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_SHARED_MUTEX_INLINE
void
_MCF_shared_mutex_init(_MCF_shared_mutex* __smutex) __MCF_NOEXCEPT
  {
    _MCF_shared_mutex __temp = __MCF_0_INIT;
    _MCF_atomic_store_pptr_rel(__smutex, &__temp);
  }

__MCF_SHARED_MUTEX_INLINE
int
_MCF_shared_mutex_lock_shared(_MCF_shared_mutex* __smutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
#if defined __OPTIMIZE__ && !defined __OPTIMIZE_SIZE__
    _MCF_shared_mutex __old = { 0, 0 };
    _MCF_shared_mutex __new = { 1, 0 };

    /* This is optimized solely for single-thread code.  */
    if(_MCF_atomic_cmpxchg_weak_pptr_acq(__smutex, &__old, &__new))
      return 0;

    /* If a timeout of zero is specified, don't block at all.  */
    if(__timeout_opt && (*__timeout_opt == 0) && (__old.__nshare == 0x3FFF))
      return -1;
#endif  /* speed */

    return _MCF_shared_mutex_lock_shared_slow(__smutex, __timeout_opt);
  }

__MCF_SHARED_MUTEX_INLINE
int
_MCF_shared_mutex_lock_exclusive(_MCF_shared_mutex* __smutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
#if defined __OPTIMIZE__ && !defined __OPTIMIZE_SIZE__
    _MCF_shared_mutex __old = { 0, 0 };
    _MCF_shared_mutex __new = { 0x3FFF, 0 };

    /* This is optimized solely for single-thread code.  */
    if(_MCF_atomic_cmpxchg_weak_pptr_acq(__smutex, &__old, &__new))
      return 0;

    /* If a timeout of zero is specified, don't block at all.  */
    if(__timeout_opt && (*__timeout_opt == 0) && (__old.__nshare != 0))
      return -1;
#endif  /* speed */

    return _MCF_shared_mutex_lock_exclusive_slow(__smutex, __timeout_opt);
  }

__MCF_SHARED_MUTEX_INLINE
void
_MCF_shared_mutex_unlock(_MCF_shared_mutex* __smutex) __MCF_NOEXCEPT
  {
#if defined __OPTIMIZE__ && !defined __OPTIMIZE_SIZE__
    _MCF_shared_mutex __old = { 1, 0 };
    _MCF_shared_mutex __new = { 0, 0 };

    /* This is optimized solely for single-thread code.  */
    if(_MCF_atomic_cmpxchg_weak_pptr_rel(__smutex, &__old, &__new))
      return;
#endif  /* speed */

    _MCF_shared_mutex_unlock_slow(__smutex);
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_SHARED_MUTEX_  */
