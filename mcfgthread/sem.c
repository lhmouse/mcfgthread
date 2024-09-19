/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_SEM_IMPORT  __MCF_DLLEXPORT
#define __MCF_SEM_INLINE  __MCF_DLLEXPORT
#include "sem.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
_MCF_sem_wait(_MCF_sem* sem, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* Decrement the counter.  */
    _MCF_sem old, new;
    old.__value = _MCF_atomic_xsub_ptr_rlx(&(sem->__value), 1);
    new.__value = old.__value - 1;

    /* If the new value is non-negative, the calling thread should leave.  */
    if(new.__value >= 0)
      return 0;

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(sem, &nt_timeout);
    while(err != 0) {
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
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have decremented the number of
       * sleeping threads and we can try incrementing it again.  */
      err = __MCF_keyed_event_wait(sem, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    return 0;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
int
_MCF_sem_signal_some(_MCF_sem* sem, intptr_t value_add)
  {
    /* Validate arguments.  */
    if(value_add <= 0)
      return (int) (value_add >> (__MCF_PTR_BITS - 1));  /* value_add ? -1 : 0  */

    /* Get the number of threads to wake up.  */
    size_t wake_num;
    _MCF_sem old, new;

    _MCF_atomic_load_pptr_rlx(&old, sem);
    do {
      if(old.__value > __MCF_SEM_VALUE_MAX - value_add)
        return -2;  /* would overflow  */

      new = old;
      wake_num = _MCF_minz(-(size_t) (old.__value & (old.__value >> (__MCF_PTR_BITS - 1))), (size_t) value_add);
      new.__value = old.__value + value_add;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(sem, &old, &new));

    __MCF_batch_release_common(sem, wake_num);
    return 0;
  }
