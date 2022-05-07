/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_COND_H_
#define __MCFGTHREAD_COND_H_

#include "fwd.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_COND_EXTERN_INLINE
#  define __MCF_COND_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* Define the condition variable struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_cond
  {
    uintptr_t __reserved : 8;

    uintptr_t __nsleep : __MCF_PTR_BITS - 8;  /* number of sleeping threads  */
#define __MCF_COND_NSLEEP_M  (UINTPTR_MAX >> 8)
  };

/* Initializes a condition variable dynamically.
 * Static ones should be initialized with `{0}`, like other structs.  */
void
_MCF_cond_init(_MCF_cond* __cond) __MCF_NOEXCEPT;

__MCF_COND_EXTERN_INLINE
void
_MCF_cond_init(_MCF_cond* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond __temp = __MCF_0_INIT;
    _MCF_atomic_store_pptr_rel(__cond, &__temp);
  }

/* Puts the current thread to sleep on a condition variable.
 *
 * The calling thread invokes the unlock callback before it goes to sleep on
 * this condition variable, and invokes the relock callback after it awakes. It
 * should unlock and relock the associated mutex, if any. The return value of
 * the unlock callback is passed to the relock callback verbatim. An unlock
 * callback may be provided regardless of a relock callback, but if a relock
 * callback is provided without an unlock callback, it is not invoked at all.
 *
 * If the `timeout` argument points to a positive integer, it denotes the wait
 * expiration time, in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the condition variable has been signaled or there is a spurious
 * wakeup, or -1 if the wait operation has timed out.  */
int
_MCF_cond_wait(_MCF_cond* __cond, _MCF_cond_unlock_callback* __unlock_opt, _MCF_cond_relock_callback* __relock_opt, intptr_t __lock_arg, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Wakes up some or all threads that have been put to sleep on this condition
 * variable.
 *
 * Returns the number of threads that have been woken up.  */
size_t
_MCF_cond_signal_some(_MCF_cond* __cond, size_t __max) __MCF_NOEXCEPT;

size_t
_MCF_cond_signal_all(_MCF_cond* __cond) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_COND_H_  */
