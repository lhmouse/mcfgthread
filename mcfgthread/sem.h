/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_SEM_
#define __MCFGTHREAD_SEM_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_SEM_IMPORT
#  define __MCF_SEM_IMPORT
#  define __MCF_SEM_INLINE  __MCF_GNU_INLINE
#endif

/** This is the counting semaphore, which takes up the same storage as a pointer.
 * The counter has the same semantics as found in an operating system book.  */
struct __MCF_sem
  {
    intptr_t __value;
  };

/** This is the maximum value of a semaphore.  */
#define __MCF_SEM_VALUE_MAX   __MCF_INTPTR_MAX

/** Define a macro for static initialization of semaphores. The argument is the
 * initial value of the semaphore, which shall not be negative.  */
#define __MCF_SEM_INIT(__value_init)  \
    { __MCF_STATIC_ASSERT_0((__value_init) >= 0) +  \
      __MCF_STATIC_ASSERT_0((__value_init) <= __MCF_SEM_VALUE_MAX) +  \
      (__value_init)  }

/** Initializes a semaphore dynamically.

 * Static ones should be initialized with `__MCF_SEM_INIT(value_init)`.

 * @param `sem` points to the semaphore to initialize.
 * @param `value_init` is the initial value of the semaphore, which shall not be
 *     negative.
 * @returns 0 if the initialization is successful, or -1 in case of an invalid
 *     argument.  */
__MCF_SEM_INLINE
int
_MCF_sem_init(_MCF_sem* __sem, intptr_t __value_init)
  __MCF_noexcept;

/** Gets the current value of a semaphore.
 *
 * If the value is negative, the absolute value of it denotes the number of
 * threads that are currently waiting on the semaphore.
 *
 * @param `sem` points to the semaphore.
 * @returns the current value of the semaphore.  */
__MCF_SEM_INLINE
intptr_t
_MCF_sem_get(const _MCF_sem* __sem)
  __MCF_noexcept;

/** Decrements the value of a semaphore.
 *
 * If the value is reduced below zero, the calling thread is suspended until
 * another thread increments the value of the semaphore.
 *
 * @param `sem` points to the semaphore.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the value has been decremented, or -1 if the operation has
 *     timed out.  */
__MCF_SEM_IMPORT
int
_MCF_sem_wait(_MCF_sem* __sem, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Increases the value of a semaphore and wake up some threads.
 *
 * @param `sem` points to the semaphore.
 * @param `value_add` is the value to add to the semaphore, which shall not be
 *     negative.
 * @returns 0 if the value has been increased successfully, -1 in case of an
 *     invalid argument, or -2 if the value would overflow.  */
__MCF_SEM_IMPORT
int
_MCF_sem_signal_some(_MCF_sem* __sem, intptr_t __value_add)
  __MCF_noexcept;

/** Increments the value of a semaphore and wakes up a thread.
 *
 * This is an inline wrapper for `_MCF_sem_signal_some(sem, 1)`.
 *
 * @param `sem` points to the semaphore.
 * @returns 0 if the value has been incremented successfully, -1 in case of an
 *     invalid argument, or -2 if the value would overflow.  */
__MCF_SEM_INLINE
int
_MCF_sem_signal(_MCF_sem* __sem)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_SEM_INLINE
int
_MCF_sem_init(_MCF_sem* __sem, intptr_t __value_init)
  __MCF_noexcept
  {
    if((__value_init < 0) || (__value_init > __MCF_SEM_VALUE_MAX))
      return -1;
    else {
      _MCF_sem __temp = __MCF_0_INIT;
      __temp.__value = __value_init;
      _MCF_atomic_store_pptr_rlx(__sem, &__temp);
      return 0;
    }
  }

__MCF_SEM_INLINE
intptr_t
_MCF_sem_get(const _MCF_sem* __sem)
  __MCF_noexcept
  {
    _MCF_sem __temp;
    _MCF_atomic_load_pptr_rlx(&__temp, __sem);
    return __temp.__value;
  }

__MCF_SEM_INLINE
int
_MCF_sem_signal(_MCF_sem* __sem)
  __MCF_noexcept
  {
    return _MCF_sem_signal_some(__sem, 1);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_SEM_  */
