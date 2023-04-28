/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_AUX_
#define __MCFGTHREAD_GTHR_AUX_

#include "fwd.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include <time.h>  /* struct timespec  */

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_GTHR_AUX_IMPORT
#  define __MCF_GTHR_AUX_IMPORT
#  define __MCF_GTHR_AUX_INLINE  __MCF_GNU_INLINE
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
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT;

/* This is an auxiliary function for converting a `__MCF_timespec` to the
 * number of milliseconds since the Unix epoch, with boundary checking.  */
__MCF_GTHR_AUX_IMPORT
int64_t
__MCF_gthr_timeout_from_timespec(const __MCF_timespec* __abs_time) __MCF_NOEXCEPT __attribute__((__pure__));

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a plain `_MCF_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a `__MCF_gthr_rc_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* This is the actual thread function for a gthread.  */
__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_thread_thunk_v2(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

__MCF_ALWAYS_INLINE
void
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmtx) __MCF_NOEXCEPT
  {
    __rmtx->__owner[0] = 0;
    __rmtx->__depth = 0;
    _MCF_mutex_init(__rmtx->__mutex);
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmtx) __MCF_NOEXCEPT
  {
    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(__rmtx->__owner) != (int32_t) _MCF_thread_self_tid())
      return -1;

    /* Increment the recursion count.  */
    __MCF_ASSERT(__rmtx->__depth < __INT32_MAX__);
    __rmtx->__depth ++;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_rc_mutex_wait(__MCF_gthr_rc_mutex* __rmtx, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
    /* Attempt to take ownership.  */
    int __err = _MCF_mutex_lock(__rmtx->__mutex, __timeout_opt);
    if(__err != 0)
      return __err;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmtx->__owner[0] == 0);
    _MCF_atomic_store_32_rlx(__rmtx->__owner, (int32_t) _MCF_thread_self_tid());
    __MCF_ASSERT(__rmtx->__depth == 0);
    __rmtx->__depth = 1;
    return 0;
  }

__MCF_ALWAYS_INLINE
void
__MCF_gthr_rc_mutex_release(__MCF_gthr_rc_mutex* __rmtx) __MCF_NOEXCEPT
  {
    /* Reduce a level of recursion.  */
    __MCF_ASSERT(__rmtx->__depth > 0);
    __rmtx->__depth --;
    if(__rmtx->__depth != 0)
      return;

    /* Give up ownership now.  */
    _MCF_atomic_store_32_rlx(__rmtx->__owner, 0);
    _MCF_mutex_unlock(__rmtx->__mutex);
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_GTHR_AUX_  */
