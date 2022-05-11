/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_SEM_EXTERN_INLINE  __MCF_DLLEXPORT
#include "sem.h"
#include "xwinnt.i"

__MCF_DLLEXPORT
int
_MCF_sem_wait(_MCF_sem* sem, const int64_t* timeout_opt)
  {
    _MCF_sem old, new;
    NTSTATUS status;

    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v2(&nt_timeout, timeout_opt);

    /* Decrement the counter.  */
    _MCF_atomic_load_pptr_rlx(&old, sem);
    do {
      new = old;
      new.__value = old.__value - 1;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(sem, &old, &new));

    /* If the new value is non-negative, the calling thread should leave.  */
    if(new.__value >= 0)
      return 0;

    /* Try waiting.  */
    status = __MCF_keyed_event_wait(sem, nt_timeout.__li);
    while(status == STATUS_TIMEOUT) {
      /* Remove myself from the wait queue. But see below...  */
      _MCF_atomic_load_pptr_rlx(&old, sem);
      do {
        if(old.__value >= 0)
          break;

        new = old;
        new.__value = old.__value + 1;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(sem, &old, &new));

      if(old.__value < 0)
        return -1;

      /* ... It is possible that a second thread has already incremented the
       * value. If this does take place, it is going to release the keyed event
       * soon. We must wait again, otherwise we get a deadlock in the second
       * thread. Again, a third thread could start waiting for this keyed event
       * before us, so we set the timeout to zero. If we time out again, the
       * third thread will have decrement the value of this semaphore and we
       * can try incrementing it again.  */
      status = __MCF_keyed_event_signal(sem, (LARGE_INTEGER[1]) __MCF_0_INIT);
    }

    /* We have got notified.  */
    return 0;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
int
_MCF_sem_signal_some(_MCF_sem* sem, intptr_t value_add)
  {
    /* Validate arguments.  */
    if(value_add == 0)
      return 0;

    if(value_add < 0)
      return -1;

    /* Get the number of threads to wake up.  */
    size_t wake_num;
    _MCF_sem old, new;

    _MCF_atomic_load_pptr_rlx(&old, sem);
    do {
      if(old.__value < 0)
        wake_num = _MCF_minz(-(size_t) old.__value, (size_t) value_add);
      else if(value_add <= __MCF_SEM_VALUE_MAX - old.__value)
        wake_num = 0;  /* no thread to wake up  */
      else
        return -2;  /* would overflow  */

      new = old;
      new.__value = old.__value + value_add;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(sem, &old, &new));

    __MCF_batch_release_common(sem, wake_num);
    return 0;
  }
