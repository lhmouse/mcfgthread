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

/* Define macros for remaining symbols, in order to prevent DLL hells.  */
#define __MCF_GTHR_DECLARE_0(a)           (void)
#define __MCF_GTHR_DECLARE_1(a)           (a __a)
#define __MCF_GTHR_DECLARE_2(a,b)         (a __a, b __b)
#define __MCF_GTHR_DECLARE_3(a,b,c)       (a __a, b __b, c __c)
#define __MCF_GTHR_DECLARE_4(a,b,c,d)     (a __a, b __b, c __c, d __d)
#define __MCF_GTHR_DECLARE_5(a,b,c,d,e)   (a __a, b __b, c __c, d __d, e __e)

#define __MCF_GTHR_FORWARD_0(a)           ()
#define __MCF_GTHR_FORWARD_1(a)           (__a)
#define __MCF_GTHR_FORWARD_2(a,b)         (__a, __b)
#define __MCF_GTHR_FORWARD_3(a,b,c)       (__a, __b, __c)
#define __MCF_GTHR_FORWARD_4(a,b,c,d)     (__a, __b, __c, __d)
#define __MCF_GTHR_FORWARD_5(a,b,c,d,e)   (__a, __b, __c, __d, __e)

#define __MCF_GTHR_1st_NX(a,...)   a
#define __MCF_GTHR_1st(...)   __MCF_GTHR_1st_NX(__VA_ARGS__)
#define __MCF_GTHR_6th_NX(a,b,c,d,e,f,...)   f
#define __MCF_GTHR_6th(...)   __MCF_GTHR_6th_NX(__VA_ARGS__)
#define __MCF_GTHR_COUNT(...)   __MCF_GTHR_6th(__VA_ARGS__, 5, 4, 3, 2, 1)
#define __MCF_GTHR_xvoid   ,
#define __MCF_GTHR_SELECT_void(tok,t,f)   __MCF_GTHR_6th(__MCF_GTHR_x##tok, 2, 3, 4, t, f)
#define __MCF_GTHR_PARAMS_SELECT_NX(f,n,ps)   f##_##n ps
#define __MCF_GTHR_PARAMS_SELECT(f,n,ps)   __MCF_GTHR_PARAMS_SELECT_NX(f,n, ps)
#define __MCF_GTHR_PARAMS_NX(f,n,t,ps)   __MCF_GTHR_PARAMS_SELECT(f,__MCF_GTHR_SELECT_void(t, 0, n), ps)
#define __MCF_GTHR_PARAMS(f,ps)   __MCF_GTHR_PARAMS_NX(f,__MCF_GTHR_COUNT ps, __MCF_GTHR_1st ps, ps)

#define __MCF_GTHR_INLINE_ALIAS(alias, RETURN, target, params, ...)  \
  __MCF_ALWAYS_INLINE  \
  RETURN  \
  alias __MCF_GTHR_PARAMS(__MCF_GTHR_DECLARE, params) __VA_ARGS__  \
      __asm__(__MCF_PPSTR(__USER_LABEL_PREFIX__) #target);  \
  \
  __MCF_ALWAYS_INLINE  \
  RETURN  \
  alias __MCF_GTHR_PARAMS(__MCF_GTHR_DECLARE, params) __VA_ARGS__  \
    {  \
      __MCF_GTHR_SELECT_void(__MCF_GTHR_1st params, ,  \
         return) target __MCF_GTHR_PARAMS(__MCF_GTHR_FORWARD, params);  \
    }

/* Define reusable types.  */
typedef struct __MCF_gthr_rc_mutex __MCF_gthr_rc_mutex;
typedef struct __MCF_gthr_thread_record __MCF_gthr_thread_record;
typedef void* __MCF_gthr_thread_procedure(void* __arg);

struct __MCF_gthr_rc_mutex
  {
    uint32_t __owner;  /* owner thread ID  */
    int __depth;  /* recursion depth  */
    _MCF_mutex __mutex;
  };

struct __MCF_gthr_thread_record
  {
    void* __result;
    __MCF_gthr_thread_procedure* __proc;
    void* __arg;
    uint8_t __joinable;
    intptr_t __reserved[2];
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
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmtx) __MCF_NOEXCEPT
  {
    __rmtx->__owner = 0;
    __rmtx->__depth = 0;
    _MCF_mutex_init(&(__rmtx->__mutex));
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmtx) __MCF_NOEXCEPT
  {
    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__rmtx->__owner)) != (int32_t) _MCF_thread_self_tid())
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
    int __err = _MCF_mutex_lock(&(__rmtx->__mutex), __timeout_opt);
    if(__err != 0)
      return __err;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__rmtx->__owner), (int32_t) _MCF_thread_self_tid());
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
    _MCF_atomic_store_32_rlx(&(__rmtx->__owner), 0);
    _MCF_mutex_unlock(&(__rmtx->__mutex));
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
