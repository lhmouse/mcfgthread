/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_AUX_
#define __MCFGTHREAD_GTHR_AUX_

#include "fwd.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include <time.h>  /* struct timespec  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_DECLSPEC_GTHR_AUX
#  define __MCF_DECLSPEC_GTHR_AUX(...)  __VA_ARGS__
#endif

/* Define macros for renaming symbols, in order to prevent DLL hells.  */
#ifdef __cplusplus
#  define __MCF_GTHR_ALIAS(alias, target)  static __MCF_CXX11(constexpr) __typeof__(target)& alias = (target)  /* no semicolon  */
#else
#  define __MCF_GTHR_ALIAS(alias, target)  static __typeof__(target)* const alias = (target)  /* no semicolon  */
#endif

/* Define reusable types.  */
typedef struct __MCF_gthr_rc_mutex __MCF_gthr_rc_mutex;
typedef struct __MCF_gthr_thread_record __MCF_gthr_thread_record;
typedef void* __MCF_gthr_thread_procedure(void* __arg);

struct __MCF_gthr_rc_mutex
  {
    uint32_t __owner[1];  /* owner thread ID  */
    int __depth;  /* recursion depth  */
    _MCF_mutex __mutex[1];
  };

struct __MCF_gthr_thread_record
  {
    void* __result;
    __MCF_gthr_thread_procedure* __proc;
    void* __arg;
    uint8_t __joinable[1];
    uintptr_t __reserved_low;
    uintptr_t __reserved_high;
  };

/* This is an auxiliary function for exception handling in `__gthread_once()`.
 * Ideally, if the target function throws exception we would like to allow
 * attempts to retry. Sadly this is not possible in standard C.  */
__MCF_DECLSPEC_GTHR_AUX(__MCF_GNU_INLINE)
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR_AUX(__MCF_GNU_INLINE)
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

/* This is an auxiliary function for converting a `struct timespec` to the
 * number of milliseconds since the Unix epoch, with boundary checking.  */
__MCF_DECLSPEC_GTHR_AUX()
int64_t
__MCF_gthr_timeout_from_timespec(const struct timespec* __abs_time) __MCF_NOEXCEPT
  __attribute__((__pure__));

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a plain `_MCF_mutex`.  */
__MCF_DECLSPEC_GTHR_AUX()
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR_AUX()
void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* These internal functions for shared mutexes.  */
__MCF_ALWAYS_INLINE
void
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmutex) __MCF_NOEXCEPT
  {
    __rmutex->__owner[0] = 0;
    __rmutex->__depth = 0;
    _MCF_mutex_init(__rmutex->__mutex);
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmutex) __MCF_NOEXCEPT
  {
    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(__rmutex->__owner) != (int32_t) _MCF_thread_self_tid())
      return -1;

    /* Increment the recursion count.  */
    __MCF_ASSERT(__rmutex->__depth < __INT32_MAX__);
    __rmutex->__depth ++;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_rc_mutex_wait(__MCF_gthr_rc_mutex* __rmutex, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
    /* Attempt to take ownership.  */
    int __err = _MCF_mutex_lock(__rmutex->__mutex, __timeout_opt);
    if(__err != 0)
      return __err;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmutex->__owner[0] == 0);
    _MCF_atomic_store_32_rlx(__rmutex->__owner, (int32_t) _MCF_thread_self_tid());
    __MCF_ASSERT(__rmutex->__depth == 0);
    __rmutex->__depth = 1;
    return 0;
  }

__MCF_ALWAYS_INLINE
void
__MCF_gthr_rc_mutex_release(__MCF_gthr_rc_mutex* __rmutex) __MCF_NOEXCEPT
  {
    /* Reduce a level of recursion.  */
    __MCF_ASSERT(__rmutex->__depth > 0);
    __rmutex->__depth --;
    if(__rmutex->__depth != 0)
      return;

    /* Give up ownership now.  */
    _MCF_atomic_store_32_rlx(__rmutex->__owner, 0);
    _MCF_mutex_unlock(__rmutex->__mutex);
  }

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a `__MCF_gthr_rc_mutex`.  */
__MCF_DECLSPEC_GTHR_AUX()
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR_AUX()
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* This is the actual thread function for a gthread.  */
__MCF_DECLSPEC_GTHR_AUX()
void
__MCF_gthr_thread_thunk_v2(_MCF_thread* __thrd) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_GTHR_AUX_  */
