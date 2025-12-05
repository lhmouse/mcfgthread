/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_GTHR_AUX_
#define __MCFGTHREAD_GTHR_AUX_

#include "fwd.h"
#include "once.h"
#include "cond.h"
#include "mutex.h"
#include "shared_mutex.h"
#include "thread.h"
#include <time.h>  /* struct timespec  */

__MCF_CXX(extern "C" {)
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
    uint8_t __magic_guid[16];
    __MCF_gthr_thread_procedure* __proc;
    void* __arg_or_result;
  };

/* These functions implement `__gthread_once()`. If `__once_fn` initiates stack
 * unwinding, by throwing an exception for example, the state of `*__once` will
 * be restored correctly.
 * FIXME: At the moment (2024-03-14) GCC does not support SEH on i686.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_call_once_seh(_MCF_once* __once, __MCF_cxa_dtor_any_ __init_proc, void* __arg)
  __MCF_MAY_THROW;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_call_once_seh_take_over(_MCF_once* __once, __MCF_cxa_dtor_any_ __init_proc, void* __arg)
  __MCF_MAY_THROW;

/* This is a type-generic macro. `__init_proc` may be of any calling convention,
 * and may take an arbitrary pointer.  */
#define __MCF_GTHR_CALL_ONCE_SEH(__once, __init_proc, __arg)  \
    __MCF_gthr_call_once_seh(__once, __MCF_CAST_PTR(__MCF_cxa_dtor_cdecl, __init_proc), __arg)

/* This is an auxiliary function for converting a `__MCF_timespec` to the
 * number of milliseconds since the Unix epoch, with boundary checking.  */
__MCF_GTHR_AUX_IMPORT __MCF_FN_PURE
int64_t
__MCF_gthr_timeout_from_timespec(const __MCF_timespec* __abs_time)
  __MCF_noexcept;

/* These are auxiliary functions to implement recursive mutexes, and are not
 * to be called directly.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept;

__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept;

__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_wait(__MCF_gthr_rc_mutex* __rmtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_release(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept;

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a plain `_MCF_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_mutex_wait(_MCF_cond* __cond, _MCF_mutex* __mtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a `_MCF_shared_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_shared_mutex_unlock_callback(intptr_t __arg)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_shared_mutex_relock_shared_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_shared_mutex_relock_exclusive_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_shared_mutex_wait_shared(_MCF_cond* __cond, _MCF_shared_mutex* __smtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_shared_mutex_wait_exclusive(_MCF_cond* __cond, _MCF_shared_mutex* __smtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a `__MCF_gthr_rc_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t __arg)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_recursive_mutex_wait(_MCF_cond* __cond, __MCF_gthr_rc_mutex* __rmtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* These are auxiliary functions for threads.  */
__MCF_GTHR_AUX_IMPORT
_MCF_thread*
__MCF_gthr_thread_create_v3(__MCF_gthr_thread_procedure* __proc, void* __arg)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_thread_join_v3(_MCF_thread* __thrd, void** __resp_opt)
  __MCF_noexcept;

__MCF_GTHR_AUX_IMPORT __MCF_NEVER_RETURN
void
__MCF_gthr_thread_exit_v3(void* __resp)
  __MCF_noexcept;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_call_once_seh(_MCF_once* __once, __MCF_cxa_dtor_any_ __init_proc, void* __arg)
  __MCF_MAY_THROW
  {
    int __err = _MCF_once_wait(__once, __MCF_nullptr);
    if(__err == 0)
      return;  /* already initialized  */

    __MCF_ASSERT(__err == 1);
    __MCF_gthr_call_once_seh_take_over(__once, __init_proc, __arg);
  }

__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept
  {
    __rmtx->__owner[0] = 0;
    __rmtx->__depth = 0;
    _MCF_mutex_init(__rmtx->__mutex);
  }

__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept
  {
    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(__rmtx->__owner) != (int32_t) _MCF_thread_self_tid())
      return -1;

    /* Increment the recursion count.  */
    __MCF_ASSERT(__rmtx->__depth < 0x7FFFFFFF);
    __rmtx->__depth ++;
    return 0;
  }

__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_wait(__MCF_gthr_rc_mutex* __rmtx, const int64_t* __timeout_opt)
  __MCF_noexcept
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

__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_release(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept
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

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_GTHR_AUX_  */
