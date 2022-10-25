/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_LIBOBJC_
#define __MCFGTHREAD_GTHR_LIBOBJC_

#include "fwd.h"
#include "gthr_aux.h"
#include <objc/thr.h>

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_DECLSPEC_GTHR_LIBOBJC_IMPORT
#  define __MCF_DECLSPEC_GTHR_LIBOBJC_IMPORT
#  define __MCF_DECLSPEC_GTHR_LIBOBJC_INLINE  __MCF_GNU_INLINE
#endif

/* Initialize the threads subsystem.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_init_thread_system(void) __MCF_NOEXCEPT
  {
    return 0;
  }

/* Close the threads subsystem.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_close_thread_system(void) __MCF_NOEXCEPT
  {
    return 0;
  }

/* Create a new thread of execution.
 * This function creates a detached thread.  */
__MCF_ALWAYS_INLINE
objc_thread_t
__gthread_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record __rec[1] = __MCF_0_INIT;
    _MCF_thread* __thrd;
    uint32_t __tid;

    __rec->__proc = __proc;
    __rec->__arg = __arg;

    __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk_v2, __rec, sizeof(*__rec));
    if(!__thrd)
      return NULL;

    __tid = __thrd->__tid;
    _MCF_thread_drop_ref_nonnull(__thrd);
    return (objc_thread_t)(uintptr_t) __tid;
  }

/* Set the current thread's priority.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_thread_set_priority(int __priority) __MCF_NOEXCEPT
  {
    _MCF_thread_priority __nt_prio;
    int __err;

    if(__priority == OBJC_THREAD_INTERACTIVE_PRIORITY)
      __nt_prio = _MCF_thread_priority_normal;
    else if(__priority == OBJC_THREAD_BACKGROUND_PRIORITY)
      __nt_prio = _MCF_thread_priority_below_normal;
    else if(__priority == OBJC_THREAD_LOW_PRIORITY)
      __nt_prio = _MCF_thread_priority_low;
    else
      return -1;

    __err = _MCF_thread_set_priority(NULL, __nt_prio);
    return (__err != 0) ? -1 : 0;
  }

/* Return the current thread's priority.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_thread_get_priority(void) __MCF_NOEXCEPT
  {
   _MCF_thread_priority __nt_prio = _MCF_thread_get_priority(NULL);

   if(__nt_prio >= _MCF_thread_priority_normal)
     return OBJC_THREAD_INTERACTIVE_PRIORITY;
   else if(__nt_prio >= _MCF_thread_priority_below_normal)
     return OBJC_THREAD_BACKGROUND_PRIORITY;
   else
     return OBJC_THREAD_LOW_PRIORITY;
  }

/* Yield our process time to another thread.  */
__MCF_ALWAYS_INLINE
void
__gthread_objc_thread_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

/* Terminate the current thread.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_thread_exit(void) __MCF_NOEXCEPT
  {
    _MCF_thread_exit();
  }

/* Returns an integer value which uniquely describes a thread.  */
__MCF_ALWAYS_INLINE
objc_thread_t
__gthread_objc_thread_id(void) __MCF_NOEXCEPT
  {
    return (objc_thread_t)(uintptr_t) _MCF_thread_self_tid();
  }

/* Sets the thread's local storage pointer.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_thread_set_data(void* __value) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(!__self)
      return -1;

    __self->__libobjc_tls_data = __value;
    return 0;
  }

/* Returns the thread's local storage pointer.  */
__MCF_ALWAYS_INLINE
void*
__gthread_objc_thread_get_data(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(!__self)
      return NULL;

    return __self->__libobjc_tls_data;
  }

/* Allocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_allocate(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    _MCF_mutex_init(__mtx);
    return 0;
  }

/* Deallocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_deallocate(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    /* libobjc calls this function only when the mutex is being held by
     * the current thread. Proceed anyway without any cleanup.  */
    (void) __objc_mutex;
    return 0;
  }

/* Grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_lock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

/* Try to grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_trylock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? -1 : 0;
  }

/* Unlock the mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_unlock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

/* Allocate a condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_allocate(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_cond_init(__cond);
    return 0;
  }

/* Deallocate a condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_deallocate(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    (void) __objc_cond;
    return 0;
  }

/* Wait on the condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_wait(objc_condition_t __objc_cond, objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

/* Wake up all threads waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_broadcast(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_cond_signal_all(__cond);
    return 0;
  }

/* Wake up one thread waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_signal(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_cond_signal(__cond);
    return 0;
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_GTHR_LIBOBJC_  */
