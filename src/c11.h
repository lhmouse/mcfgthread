/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_C11_
#define __MCFGTHREAD_C11_

#include "fwd.h"
#include "gthr_aux.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define wrappers as required by the ISO/IEC C11 standard.  */
#ifndef __MCF_DECLSPEC_C11
#  define __MCF_DECLSPEC_C11(...)  __VA_ARGS__
#endif

/* N1570 7.26.1 Introduction  */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201103L)
#  define thread_local  _Thread_local
#endif

#define ONCE_FLAG_INIT  __MCF_0_INIT
#define TSS_DTOR_ITERATIONS  LONG_MAX  /* no limit  */

/* Define <threads.h> types. These usually map to our APIs directly, except
 * the mutex.  */
typedef struct __MCF_c11_thread_record __MCF_c11_thread_record;
typedef int __MCF_c11_thread_procedure(void* __arg);

struct __MCF_c11_mutex
  {
    uint8_t __type;  /* bit mask of `__MCF_mtx_type`  */
    uint8_t __reserved_1;
    uint16_t __reserved_2;
    uint32_t __owner;  /* owner thread ID  */
    int __depth;  /* recursion depth  */
    _MCF_mutex __mutex;
  };

struct __MCF_c11_thread_record
  {
    int __result;
    __MCF_c11_thread_procedure* __proc;
    void* __arg;
    uint8_t __joinable;
    intptr_t __reserved[2];
  };

typedef __MCF_c11_thread_procedure* thrd_start_t;
typedef _MCF_thread* thrd_t;
typedef _MCF_tls_key* tss_t;
typedef _MCF_tls_dtor* tss_dtor_t;

typedef _MCF_once once_flag;
typedef _MCF_cond cnd_t;
typedef struct __MCF_c11_mutex mtx_t;

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a `mtx_t`.  */
__MCF_DECLSPEC_C11()
intptr_t
__MCF_c11_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11()
void
__MCF_c11_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* This is the actual thread function for a C11 thread.  */
__MCF_DECLSPEC_C11()
void
__MCF_c11_thread_thunk_v2(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Define enumeration constants.  */
enum __MCF_mtx_type
  {
    mtx_plain       = 0,
    mtx_recursive   = 1,
    mtx_timed       = 2,
    __MCF_mtx_end
  };

enum __MCF_thrd_error
  {
    thrd_success    = 0,
    thrd_busy       = 1,
    thrd_error      = 2,
    thrd_nomem      = 3,
    thrd_timedout   = 4,
    __MCF_thrd_end
  };

/* 7.26.2.1 The call_once function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_call_once(once_flag* __flag, void __init_func(void));

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_call_once(once_flag* __flag, void __init_func(void))
  {
    _MCF_once* __cleanup __attribute__((__cleanup__(__MCF_gthr_unonce))) = NULL;

    if(_MCF_once_wait(__flag, NULL) == 0)
      return;

    __cleanup = __flag;
    __init_func();
    __cleanup = NULL;

    _MCF_once_release(__flag);
  }

static __inline__
void
call_once(once_flag* __flag, void __init_func(void))
  {
    __MCF_c11_call_once(__flag, __init_func);
  }

/* 7.26.3.1 The cnd_broadcast function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return thrd_success;
  }

static __inline__
int
cnd_broadcast(cnd_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_c11_cnd_broadcast(__cond);
  }

/* 7.26.3.2 The cnd_destroy function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
  }

static __inline__
void
cnd_destroy(cnd_t* __cond) __MCF_NOEXCEPT
  {
    __MCF_c11_cnd_destroy(__cond);
  }

/* 7.26.3.3 The cnd_init function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_init(cnd_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_init(cnd_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return thrd_success;
  }

static __inline__
int
cnd_init(cnd_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_c11_cnd_init(__cond);
  }

/* 7.26.3.4 The cnd_signal function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal(__cond);
    return thrd_success;
  }

static __inline__
int
cnd_signal(cnd_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_c11_cnd_signal(__cond);
  }

/* 7.26.3.5 The cnd_timedwait function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const struct timespec* __ts) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const struct timespec* __ts) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__ts);
    int __err = _MCF_cond_wait(__cond, __MCF_c11_mutex_unlock_callback, __MCF_c11_mutex_relock_callback, (intptr_t) __mtx, &__timeout);
    return (__err != 0) ? thrd_timedout : thrd_success;
  }

static __inline__
int
cnd_timedwait(cnd_t* __cond, mtx_t* __mtx, const struct timespec* __ts) __MCF_NOEXCEPT
  {
    return __MCF_c11_cnd_timedwait(__cond, __mtx, __ts);
  }

/* 7.26.3.6 The cnd_wait function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_c11_mutex_unlock_callback, __MCF_c11_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return thrd_success;
  }

static __inline__
int
cnd_wait(cnd_t* __cond, mtx_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_c11_cnd_wait(__cond, __mtx);
  }

/* 7.26.4.1 The mtx_destroy function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
  }

static __inline__
void
mtx_destroy(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    __MCF_c11_mtx_destroy(__mtx);
  }

/* 7.26.4.2 The mtx_init function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_init(mtx_t* __mtx, int __type) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
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
        __mtx->__owner = 0;
        __mtx->__depth = 0;
        _MCF_mutex_init(&(__mtx->__mutex));
        return thrd_success;
    }
  }

static __inline__
int
mtx_init(mtx_t* __mtx, int __type) __MCF_NOEXCEPT
  {
    return __MCF_c11_mtx_init(__mtx, __type);
  }

/* 7.26.4.3 The mtx_lock function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    int __err;

    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__mtx->__owner)) == (int32_t) __my_tid) {
      /* Fail if the mutex is not recursive.  */
      if(!(__mtx->__type & mtx_recursive))
        return thrd_error;

      /* Update the recursion count.  */
      __MCF_ASSERT(__mtx->__depth < __INT32_MAX__);
      __mtx->__depth ++;
      return thrd_success;
    }

    /* Attempt to take ownership.  */
    __err = _MCF_mutex_lock(&(__mtx->__mutex), NULL);
    __MCF_ASSERT(__err == 0);

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__mtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__mtx->__owner), (int32_t) __my_tid);
    __MCF_ASSERT(__mtx->__depth == 0);
    __mtx->__depth = 1;
    return thrd_success;
  }

static __inline__
int
mtx_lock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_c11_mtx_lock(__mtx);
  }

/* 7.26.4.4 The mtx_timedlock function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_timedlock(mtx_t* __mtx, const struct timespec* __ts) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_timedlock(mtx_t* __mtx, const struct timespec* __ts) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    int64_t __timeout = 0;
    int __err;

    /* Fail if the mutex does not support timeout.  */
    if(!(__mtx->__type & mtx_timed))
      return thrd_error;

    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__mtx->__owner)) == (int32_t) __my_tid) {
      /* Fail if the mutex is not recursive.  */
      if(!(__mtx->__type & mtx_recursive))
        return thrd_error;

      /* Update the recursion count.  */
      __MCF_ASSERT(__mtx->__depth < __INT32_MAX__);
      __mtx->__depth ++;
      return thrd_success;
    }

    /* Attempt to take ownership.  */
    __timeout = __MCF_gthr_timeout_from_timespec(__ts);
    __err = _MCF_mutex_lock(&(__mtx->__mutex), &__timeout);
    if(__err != 0)
      return thrd_timedout;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__mtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__mtx->__owner), (int32_t) __my_tid);
    __MCF_ASSERT(__mtx->__depth == 0);
    __mtx->__depth = 1;
    return thrd_success;
  }

static __inline__
int
mtx_timedlock(mtx_t* __mtx, const struct timespec* __ts) __MCF_NOEXCEPT
  {
    return __MCF_c11_mtx_timedlock(__mtx, __ts);
  }

/* 7.26.4.5 The mtx_trylock function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    int64_t __timeout = 0;
    int __err;

    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__mtx->__owner)) == (int32_t) __my_tid) {
      /* Fail if the mutex is not recursive.  */
      if(!(__mtx->__type & mtx_recursive))
        return thrd_error;

      /* Update the recursion count.  */
      __MCF_ASSERT(__mtx->__depth < __INT32_MAX__);
      __mtx->__depth ++;
      return thrd_success;
    }

    /* Attempt to take ownership.  */
    __err = _MCF_mutex_lock(&(__mtx->__mutex), &__timeout);
    if(__err != 0)
      return thrd_busy;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__mtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__mtx->__owner), (int32_t) __my_tid);
    __MCF_ASSERT(__mtx->__depth == 0);
    __mtx->__depth = 1;
    return thrd_success;
  }

static __inline__
int
mtx_trylock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_c11_mtx_trylock(__mtx);
  }

/* 7.26.4.6 The mtx_unlock function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();

    /* Check for errors.  */
    if(__mtx->__owner != __my_tid)
      return thrd_error;

    if(__mtx->__depth <= 0)
      return thrd_error;

    /* Reduce a level of recursion.  */
    __mtx->__depth --;
    if(__mtx->__depth != 0)
      return thrd_success;

    /* Give up ownership now.  */
    _MCF_atomic_store_32_rlx(&(__mtx->__owner), 0);
    _MCF_mutex_unlock(&(__mtx->__mutex));
    return thrd_success;
  }

static __inline__
int
mtx_unlock(mtx_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_c11_mtx_unlock(__mtx);
  }

/* 7.26.5.1 The thrd_create function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_thrd_create(thrd_t* __thrd, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_thrd_create(thrd_t* __thrdp, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_c11_thread_record __rec[1];
    _MCF_thread* __thrd;

    __rec->__proc = __proc;
    __rec->__arg = __arg;
    __rec->__joinable = true;

    __thrd = _MCF_thread_new(__MCF_c11_thread_thunk_v2, __rec, sizeof(*__rec));
    *__thrdp = __thrd;
    return (__thrd == NULL) ? thrd_nomem : thrd_success;
  }

static __inline__
int
thrd_create(thrd_t* __thrd, thrd_start_t __proc, void* __arg) __MCF_NOEXCEPT
  {
    return __MCF_c11_thrd_create(__thrd, __proc, __arg);
  }

/* 7.26.5.2 The thrd_current function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
thrd_t
__MCF_c11_thrd_current(void) __MCF_NOEXCEPT
  __attribute__((__const__, __returns_nonnull__));

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
thrd_t
__MCF_c11_thrd_current(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    __MCF_CHECK(__self);
    return __self;
  }

static __inline__
thrd_t
thrd_current(void) __MCF_NOEXCEPT
  {
    return __MCF_c11_thrd_current();
  }

/* 7.26.5.3 The thrd_detach function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_thrd_detach(thrd_t __thrd) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
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
    if(_MCF_atomic_xchg_8_rlx(&(__rec->__joinable), 0) == 0)
      return thrd_error;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return thrd_success;
  }

static __inline__
int
thrd_detach(thrd_t __thrd) __MCF_NOEXCEPT
  {
    return __MCF_c11_thrd_detach(__thrd);
  }

/* 7.26.5.4 The thrd_equal function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
__MCF_CXX11(constexpr)
int
__MCF_c11_thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
__MCF_CXX11(constexpr)
int
__MCF_c11_thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

static __inline__
int
thrd_equal(thrd_t __t1, thrd_t __t2) __MCF_NOEXCEPT
  {
    return __MCF_c11_thrd_equal(__t1, __t2);
  }

/* 7.26.5.5 The thrd_exit function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_thrd_exit(int __res) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_thrd_exit(int __result) __MCF_NOEXCEPT
  {
    __MCF_c11_thread_record* __rec = NULL;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    _MCF_thread* __self = _MCF_thread_self();
    if(__self && (__self->__proc == __MCF_c11_thread_thunk_v2))
      __rec = (__MCF_c11_thread_record*) _MCF_thread_get_data(__self);

    /* Set the result.  */
    if(__rec)
      __rec->__result = __result;

    /* Exit, even in the case of a foreign thread. Unlike `ExitThread()`, if
     * the last thread exits, the current process exits with zero.  */
    _MCF_thread_exit();
  }

static __inline__
void
thrd_exit(int __res) __MCF_NOEXCEPT
  {
    __MCF_c11_thrd_exit(__res);
  }

/* 7.26.5.6 The thrd_join function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_thrd_join(thrd_t __thrd, int* __resp_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
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

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(&(__rec->__joinable), 0) == 0)
      return thrd_error;

    /* Wait for it.  */
    if(__thrd == _MCF_thread_self())
      return thrd_error;

    __err = _MCF_thread_wait(__thrd, NULL);
    __MCF_ASSERT(__err == 0);

    if(__resp_opt)
      *__resp_opt = __rec->__result;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return thrd_success;
  }

static __inline__
int
thrd_join(thrd_t __thrd, int* __resp_opt) __MCF_NOEXCEPT
  {
    return __MCF_c11_thrd_join(__thrd, __resp_opt);
  }

/* 7.26.5.7 The thrd_sleep function  */
__MCF_DECLSPEC_C11()
int
__MCF_c11_thrd_sleep(const struct timespec* __dur, struct timespec* __rem_opt) __MCF_NOEXCEPT;

static __inline__
int
thrd_sleep(const struct timespec* __dur, struct timespec* __rem_opt) __MCF_NOEXCEPT
  {
    return __MCF_c11_thrd_sleep(__dur, __rem_opt);
  }

/* 7.26.5.8 The thrd_yield function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_thrd_yield(void) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_thrd_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

static __inline__
void
thrd_yield(void) __MCF_NOEXCEPT
  {
    __MCF_c11_thrd_yield();
  }

/* 7.26.6.1 The tss_create function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == NULL) ? thrd_nomem : thrd_success;
  }

static __inline__
int
tss_create(tss_t* __keyp, tss_dtor_t __dtor_opt) __MCF_NOEXCEPT
  {
    return __MCF_c11_tss_create(__keyp, __dtor_opt);
  }

/* 7.26.6.2 The tss_delete function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_tss_delete(tss_t __key) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void
__MCF_c11_tss_delete(tss_t __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
  }

static __inline__
void
tss_delete(tss_t __key) __MCF_NOEXCEPT
  {
    __MCF_c11_tss_delete(__key);
  }

/* 7.26.6.3 The tss_get function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void*
__MCF_c11_tss_get(tss_t __key) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
void*
__MCF_c11_tss_get(tss_t __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

static __inline__
void*
tss_get(tss_t __key) __MCF_NOEXCEPT
  {
    return __MCF_c11_tss_get(__key);
  }

/* 7.26.6.4 The tss_set function  */
__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_C11(__MCF_GNU_INLINE)
int
__MCF_c11_tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? thrd_error : thrd_success;
  }

static __inline__
int
tss_set(tss_t __key, void* __val_opt) __MCF_NOEXCEPT
  {
    return __MCF_c11_tss_set(__key, __val_opt);
  }

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_C11_  */
