/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_SEM_
#define __MCFGTHREAD_SEM_

#include "fwd.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_SEM_EXTERN_INLINE
#  define __MCF_SEM_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* Define the semaphore struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_sem
  {
    intptr_t __value;
  };

/* This is the maximum value of a semaphore.  */
#define __MCF_SEM_VALUE_MAX   PTRDIFF_MAX

/* Define a macro for static initialization of semaphores. The argument is the
 * initial value of the semaphore, which shall not be negative.  */
#define __MCF_SEM_INIT(__value_init)  \
    { __MCF_STATIC_ASSERT((__value_init) >= 0) +  \
      __MCF_STATIC_ASSERT((__value_init) <= __MCF_SEM_VALUE_MAX) +  \
      (__value_init)  }

/* Initializes a semaphore dynamically. The argument is the initial value of
 * the semaphore, which shall not be negative.
 * Static ones should be initialized with `__MCF_SEM_INIT(__value_init)`.
 *
 * Returns 0 if the initialization is successful, or -1 in case of invalid
 * arguments.  */
int
_MCF_sem_init(_MCF_sem* __sem, intptr_t __value_init) __MCF_NOEXCEPT;

__MCF_SEM_EXTERN_INLINE
int
_MCF_sem_init(_MCF_sem* __sem, intptr_t __value_init) __MCF_NOEXCEPT
  {
    if(__value_init < 0)
      return -1;

    _MCF_sem __temp = { __value_init };
    _MCF_atomic_store_pptr_rel(__sem, &__temp);
    return 0;
  }

/* Gets the current value of a semaphore.
 *
 * Returns the current value as a signed integer. If the value is negative, its
 * absolute value denotes the number of threads that have been suspended on
 * this semaphore.  */
intptr_t
_MCF_sem_get(const _MCF_sem* __sem) __MCF_NOEXCEPT;

__MCF_SEM_EXTERN_INLINE
intptr_t
_MCF_sem_get(const _MCF_sem* __sem) __MCF_NOEXCEPT
  {
    _MCF_sem __temp;
    _MCF_atomic_load_pptr_rlx(&__temp, __sem);
    return __temp.__value;
  }

/* Decrements the value of a semaphore. If the result is negative, the calling
 * thread will be suspended.
 *
 * If the `timeout` argument points to a positive integer, it denotes the wait
 * expiration time, in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the value had been decremented and the calling thread has been
 * woken up by another thread, or -1 if the operation has timed out.  */
int
_MCF_sem_wait(_MCF_sem* __sem, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Increases the value of a semaphore by the specified value. If the value was
 * negative before the call, a waiting thread is woken up. The argument shall
 * not be negative.
 *
 * Returns 0 if the value has been updated successfully, or -1 in case of
 * invalid arguments, or -2 if the result would overflow.  */
int
_MCF_sem_signal_some(_MCF_sem* __sem, intptr_t __value_add) __MCF_NOEXCEPT;

/* This function is equivalent to `_MCF_sem_signal_some(sem, 1)`.  */
int
_MCF_sem_signal(_MCF_sem* __sem) __MCF_NOEXCEPT;

__MCF_SEM_EXTERN_INLINE
int
_MCF_sem_signal(_MCF_sem* __sem) __MCF_NOEXCEPT
  {
    return _MCF_sem_signal_some(__sem, 1);
  }

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_SEM_  */
