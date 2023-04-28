/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_C11_
#define __MCFGTHREAD_C11_

#include "fwd.h"
#include "gthr_aux.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_C11_IMPORT
#  define __MCF_C11_IMPORT
#  define __MCF_C11_INLINE  __MCF_GNU_INLINE
#endif

/* N1570 7.26.1 Introduction  */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201103L)
#  define thread_local  _Thread_local
#endif

#define ONCE_FLAG_INIT  __MCF_0_INIT
#define TSS_DTOR_ITERATIONS  LONG_MAX  /* no limit  */

/* Define <threads.h> types. These usually map to our APIs directly, except
 * the mutex.  */
typedef struct __MCF_c11_mutex __MCF_c11_mutex;
typedef struct __MCF_c11_thread_record __MCF_c11_thread_record;
typedef int __MCF_c11_thread_procedure(void* __arg);

struct __MCF_c11_mutex
  {
    uint8_t __type;  /* bit mask of `__MCF_mtx_type`  */
    __MCF_gthr_rc_mutex __rc_mtx[1];
  };

struct __MCF_c11_thread_record
  {
    int __result;
    __MCF_c11_thread_procedure* __proc;
    void* __arg;
    uint8_t __joinable[1];
    uintptr_t __reserved_low;
    uintptr_t __reserved_high;
  };

typedef __MCF_c11_thread_procedure* thrd_start_t;
typedef _MCF_thread* thrd_t;
typedef _MCF_tls_key* tss_t;
typedef _MCF_tls_dtor* tss_dtor_t;

typedef _MCF_once once_flag;
typedef _MCF_cond cnd_t;
typedef __MCF_c11_mutex mtx_t;

/* This is the actual thread function for a C11 thread.  */
__MCF_C11_IMPORT
void
__MCF_c11_thread_thunk_v2(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Define enumeration constants.  */
enum __MCF_mtx_type
  {
    mtx_plain           = 0,
    mtx_recursive       = 1,
    mtx_timed           = 2,
    __MCF_mtx_type_end
  };

enum __MCF_thrd_error
  {
    thrd_success          = 0,
    thrd_busy             = 1,
    thrd_error            = 2,
    thrd_nomem            = 3,
    thrd_timedout         = 4,
    __MCF_thrd_error_end
  };

/* 7.26.2.1 The call_once function  */
__MCF_C11_INLINE
void
__MCF_c11_call_once(once_flag* __flag, __MCF_once_callback* __init_func);

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void call_once(once_flag* __flag, __MCF_once_callback* __init_func) __MCF_ASM_CALL(__MCF_c11_call_once);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void call_once(once_flag* __flag, __MCF_once_callback* __init_func) { __MCF_c11_call_once(__flag, __init_func);  }
#  endif  /* __clang__  */
#endif

/* 7.26.3.1 The cnd_broadcast function  */
__MCF_C11_INLINE
int
__MCF_c11_cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_cnd_broadcast);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT { return __MCF_c11_cnd_broadcast(__cond);  }
#  endif  /* __clang__  */
#endif

/* 7.26.3.2 The cnd_destroy function  */
__MCF_C11_INLINE
void
__MCF_c11_cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_cnd_destroy);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT { __MCF_c11_cnd_destroy(__cond);  }
#  endif  /* __clang__  */
#endif

/* 7.26.3.3 The cnd_init function  */
__MCF_C11_INLINE
int
__MCF_c11_cnd_init(cnd_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int cnd_init(cnd_t* __cond) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_cnd_init);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int cnd_init(cnd_t* __cond) __MCF_NOEXCEPT { return __MCF_c11_cnd_init(__cond);  }
#  endif  /* __clang__  */
#endif

/* 7.26.3.4 The cnd_signal function  */
__MCF_C11_INLINE
int
__MCF_c11_cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_cnd_signal);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT { return __MCF_c11_cnd_signal(__cond);  }
#  endif  /* __clang__  */
#endif

/* 7.26.3.5 The cnd_timedwait function  */
__MCF_C11_INLINE
int
__MCF_c11_cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_cnd_timedwait);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT { return __MCF_c11_cnd_timedwait(__cond, __mtx, __ts);  }
#  endif  /* __clang__  */
#endif

/* 7.26.3.6 The cnd_wait function  */
__MCF_C11_INLINE
int
__MCF_c11_cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_cnd_wait);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT { return __MCF_c11_cnd_wait(__cond, __mtx);  }
#  endif  /* __clang__  */
#endif

/* 7.26.4.1 The mtx_destroy function  */
__MCF_C11_INLINE
void
__MCF_c11_mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_mtx_destroy);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT { __MCF_c11_mtx_destroy(__mtx);  }
#  endif  /* __clang__  */
#endif

/* 7.26.4.2 The mtx_init function  */
__MCF_C11_INLINE
int
__MCF_c11_mtx_init(mtx_t* __mtx, int __type) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int mtx_init(mtx_t* __mtx, int __type) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_mtx_init);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int mtx_init(mtx_t* __mtx, int __type) __MCF_NOEXCEPT { return __MCF_c11_mtx_init(__mtx, __type);  }
#  endif  /* __clang__  */
#endif

/* 7.26.4.3 The mtx_lock function  */
__MCF_C11_INLINE
int
__MCF_c11_mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_mtx_lock);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT { return __MCF_c11_mtx_lock(__mtx);  }
#  endif  /* __clang__  */
#endif

/* 7.26.4.4 The mtx_timedlock function  */
__MCF_C11_INLINE
int
__MCF_c11_mtx_timedlock(mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int mtx_timedlock(mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_mtx_timedlock);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int mtx_timedlock(mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT { return __MCF_c11_mtx_timedlock(__mtx, __ts);  }
#  endif  /* __clang__  */
#endif

/* 7.26.4.5 The mtx_trylock function  */
__MCF_C11_INLINE
int
__MCF_c11_mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_mtx_trylock);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT { return __MCF_c11_mtx_trylock(__mtx);  }
#  endif  /* __clang__  */
#endif

/* 7.26.4.6 The mtx_unlock function  */
__MCF_C11_INLINE
int
__MCF_c11_mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_mtx_unlock);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT { return __MCF_c11_mtx_unlock(__mtx);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.1 The thrd_create function  */
__MCF_C11_INLINE
int
__MCF_c11_thrd_create(thrd_t* __thrd, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int thrd_create(thrd_t* __thrd, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_create);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int thrd_create(thrd_t* __thrd, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT { return __MCF_c11_thrd_create(__thrd, __proc, __arg);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.2 The thrd_current function  */
__MCF_C11_INLINE
thrd_t
__MCF_c11_thrd_current(void) __MCF_NOEXCEPT __attribute__((__const__, __returns_nonnull__));

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE thrd_t thrd_current(void) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_current);
#  ifndef __clang__
__MCF_ALWAYS_INLINE thrd_t thrd_current(void) __MCF_NOEXCEPT { return __MCF_c11_thrd_current();  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.3 The thrd_detach function  */
__MCF_C11_INLINE
int
__MCF_c11_thrd_detach(thrd_t __thrd) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int thrd_detach(thrd_t __thrd) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_detach);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int thrd_detach(thrd_t __thrd) __MCF_NOEXCEPT { return __MCF_c11_thrd_detach(__thrd);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.4 The thrd_equal function  */
__MCF_C11_INLINE __MCF_CXX11(constexpr)
int
__MCF_c11_thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT __attribute__((__pure__));

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_equal);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT { return __MCF_c11_thrd_equal(__t1, __t2);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.5 The thrd_exit function  */
__MCF_C11_INLINE
void
__MCF_c11_thrd_exit(int __res) __MCF_NOEXCEPT __attribute__((__noreturn__));

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void thrd_exit(int __res) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_exit);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void thrd_exit(int __res) __MCF_NOEXCEPT { __MCF_c11_thrd_exit(__res);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.6 The thrd_join function  */
__MCF_C11_INLINE
int
__MCF_c11_thrd_join(thrd_t __thrd, int* __resp_opt) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int thrd_join(thrd_t __thrd, int* __resp_opt) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_join);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int thrd_join(thrd_t __thrd, int* __resp_opt) __MCF_NOEXCEPT { return __MCF_c11_thrd_join(__thrd, __resp_opt);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.7 The thrd_sleep function  */
__MCF_C11_IMPORT
int
__MCF_c11_thrd_sleep(const __MCF_timespec* __dur, __MCF_timespec* __rem_opt) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int thrd_sleep(const __MCF_timespec* __dur, __MCF_timespec* __rem_opt) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_sleep);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int thrd_sleep(const __MCF_timespec* __dur, __MCF_timespec* __rem_opt) __MCF_NOEXCEPT { return __MCF_c11_thrd_sleep(__dur, __rem_opt);  }
#  endif  /* __clang__  */
#endif

/* This is a non-standard extension that takes a time point instead of a
 * duration. No remaining time is returned.  */
__MCF_C11_IMPORT
int
__MCF_c11__thrd_sleep_until(const __MCF_timespec* __ts) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int _thrd_sleep_until(const __MCF_timespec* __ts) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11__thrd_sleep_until);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int _thrd_sleep_until(const __MCF_timespec* __ts) __MCF_NOEXCEPT { return __MCF_c11__thrd_sleep_until(__ts);  }
#  endif  /* __clang__  */
#endif

/* 7.26.5.8 The thrd_yield function  */
__MCF_C11_INLINE
void
__MCF_c11_thrd_yield(void) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void thrd_yield(void) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_thrd_yield);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void thrd_yield(void) __MCF_NOEXCEPT { __MCF_c11_thrd_yield();  }
#  endif  /* __clang__  */
#endif

/* 7.26.6.1 The tss_create function  */
__MCF_C11_INLINE
int
__MCF_c11_tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_tss_create);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT { return __MCF_c11_tss_create(__keyp, __dtor_opt);  }
#  endif  /* __clang__  */
#endif

/* 7.26.6.2 The tss_delete function  */
__MCF_C11_INLINE
void
__MCF_c11_tss_delete(tss_t __key) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void tss_delete(tss_t __key) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_tss_delete);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void tss_delete(tss_t __key) __MCF_NOEXCEPT { __MCF_c11_tss_delete(__key);  }
#  endif  /* __clang__  */
#endif

/* 7.26.6.3 The tss_get function  */
__MCF_C11_INLINE
void*
__MCF_c11_tss_get(tss_t __key) __MCF_NOEXCEPT __attribute__((__pure__));

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE void* tss_get(tss_t __key) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_tss_get);
#  ifndef __clang__
__MCF_ALWAYS_INLINE void* tss_get(tss_t __key) __MCF_NOEXCEPT { return __MCF_c11_tss_get(__key);  }
#  endif  /* __clang__  */
#endif

/* 7.26.6.4 The tss_set function  */
__MCF_C11_INLINE
int
__MCF_c11_tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT;

#ifndef __MCF_C11_NO_ALIASES
__MCF_ALWAYS_INLINE int tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT __MCF_ASM_CALL(__MCF_c11_tss_set);
#  ifndef __clang__
__MCF_ALWAYS_INLINE int tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT { return __MCF_c11_tss_set(__key, __val_opt);  }
#  endif  /* __clang__  */
#endif

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_C11_INLINE
void
__MCF_c11_call_once(once_flag* __flag, __MCF_once_callback* __init_func)
  {
    _MCF_once* __cleanup __attribute__((__cleanup__(__MCF_gthr_unonce))) = NULL;

    if(_MCF_once_wait(__flag, NULL) == 0)
      return;

    __cleanup = __flag;
    __init_func();
    __cleanup = NULL;
    _MCF_once_release(__flag);
  }

__MCF_C11_INLINE
int
__MCF_c11_cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return thrd_success;
  }

__MCF_C11_INLINE
void
__MCF_c11_cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
  }

__MCF_C11_INLINE
int
__MCF_c11_cnd_init(cnd_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal(__cond);
    return thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__ts);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_recursive_mutex_unlock_callback, __MCF_gthr_recursive_mutex_relock_callback, (intptr_t) __mtx->__rc_mtx, &__timeout);
    return (__err != 0) ? thrd_timedout : thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_recursive_mutex_unlock_callback, __MCF_gthr_recursive_mutex_relock_callback, (intptr_t) __mtx->__rc_mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return thrd_success;
  }

__MCF_C11_INLINE
void
__MCF_c11_mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
  }

__MCF_C11_INLINE
int
__MCF_c11_mtx_init(mtx_t* __mtx, int __type) __MCF_NOEXCEPT
  {
    switch(__type) {
      default:
        /* Invalid `__type` specified.  */
        return thrd_error;

      case mtx_plain:
      case mtx_timed:
      case mtx_plain | mtx_recursive:
      case mtx_timed | mtx_recursive:
        /* Initialize an unowned mutex.  */
        __mtx->__type = (uint8_t) __type;
        __MCF_gthr_rc_mutex_init(__mtx->__rc_mtx);
        return thrd_success;
    }
  }

__MCF_ALWAYS_INLINE
int
__MCF_c11_mtx_check_recursion(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    /* Check for recursion.  */
    int __err = __MCF_gthr_rc_mutex_recurse(__mtx->__rc_mtx);
    if(__err != 0)
      return thrd_busy;

    /* If recursion has happened but the mutex is not recursive, undo the
     * operation, and fail.  */
    if(!(__mtx->__type & mtx_recursive)) {
      __mtx->__rc_mtx[0].__depth --;
      return thrd_error;
    }

    /* Accept the recursion.  */
    return thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = __MCF_c11_mtx_check_recursion(__mtx);
    if(__err != thrd_busy)
      return __err;

    __err = __MCF_gthr_rc_mutex_wait(__mtx->__rc_mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_mtx_timedlock(mtx_t* __mtx, const __MCF_timespec* __ts) __MCF_NOEXCEPT
  {
    int64_t __timeout;
    int __err;

    if(!(__mtx->__type & mtx_timed))
      return thrd_error;

    __err = __MCF_c11_mtx_check_recursion(__mtx);
    if(__err != thrd_busy)
      return __err;

    __timeout = __MCF_gthr_timeout_from_timespec(__ts);
    __err = __MCF_gthr_rc_mutex_wait(__mtx->__rc_mtx, &__timeout);
    return (__err != 0) ? thrd_timedout : thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout;
    int __err;

    __err = __MCF_c11_mtx_check_recursion(__mtx);
    if(__err != thrd_busy)
      return __err;

    __timeout = 0;
    __err = __MCF_gthr_rc_mutex_wait(__mtx->__rc_mtx, &__timeout);
    return (__err != 0) ? thrd_busy : thrd_success;
  }

__MCF_C11_INLINE
int
__MCF_c11_mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_release(__mtx->__rc_mtx);
    return 0;
  }

__MCF_C11_INLINE
int
__MCF_c11_thrd_create(thrd_t* __thrdp, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_c11_thread_record __rec[1] = __MCF_0_INIT;
    _MCF_thread* __thrd;

    __rec->__proc = __proc;
    __rec->__arg = __arg;
    __rec->__joinable[0] = 1;

    __thrd = _MCF_thread_new(__MCF_c11_thread_thunk_v2, __rec, sizeof(*__rec));
    *__thrdp = __thrd;
    return (__thrd == NULL) ? thrd_nomem : thrd_success;
  }

__MCF_C11_INLINE
thrd_t
__MCF_c11_thrd_current(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    __MCF_CHECK(__self);
    return __self;
  }

__MCF_C11_INLINE
int
__MCF_c11_thrd_detach(thrd_t __thrd) __MCF_NOEXCEPT
  {
    __MCF_c11_thread_record* __rec;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_c11_thread_thunk_v2)
      return thrd_error;

    __rec = (__MCF_c11_thread_record*) _MCF_thread_get_data(__thrd);

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(__rec->__joinable, 0) == 0)
      return thrd_error;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return thrd_success;
  }

__MCF_C11_INLINE __MCF_CXX11(constexpr)
int
__MCF_c11_thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

__MCF_C11_INLINE
void
__MCF_c11_thrd_exit(int __result) __MCF_NOEXCEPT
  {
    __MCF_c11_thread_record* __rec;
    _MCF_thread* __self = _MCF_thread_self();

    if(!__self)
      _MCF_thread_exit();

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type. The current thread
     * shall terminate even if it is foreign.  */
    if(__self->__proc != __MCF_c11_thread_thunk_v2)
      _MCF_thread_exit();

    /* Set the exit status and exit. Unlike `ExitThread()`, if the last
     * thread exits, the current process exits with zero.  */
    __rec = (__MCF_c11_thread_record*) _MCF_thread_get_data(__self);
    __rec->__result = __result;
    _MCF_thread_exit();
  }

__MCF_C11_INLINE
int
__MCF_c11_thrd_join(thrd_t __thrd, int* __resp_opt) __MCF_NOEXCEPT
  {
    __MCF_c11_thread_record* __rec;
    int __err;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_c11_thread_thunk_v2)
      return thrd_error;

    __rec = (__MCF_c11_thread_record*) _MCF_thread_get_data(__thrd);

    /* Joining with the calling thread itself would result in deadlocks.  */
    if(__thrd->__tid == _MCF_thread_self_tid())
      return thrd_error;

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(__rec->__joinable, 0) == 0)
      return thrd_error;

    __err = _MCF_thread_wait(__thrd, NULL);
    __MCF_ASSERT(__err == 0);
    __MCF_SET_IF(__resp_opt, __rec->__result);

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return thrd_success;
  }

__MCF_C11_INLINE
void
__MCF_c11_thrd_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

__MCF_C11_INLINE
int
__MCF_c11_tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == NULL) ? thrd_nomem : thrd_success;
  }

__MCF_C11_INLINE
void
__MCF_c11_tss_delete(tss_t __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
  }

__MCF_C11_INLINE
void*
__MCF_c11_tss_get(tss_t __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

__MCF_C11_INLINE
int
__MCF_c11_tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? thrd_error : thrd_success;
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_C11_  */
