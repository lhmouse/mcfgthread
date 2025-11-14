/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_COND_IMPORT  __MCF_DLLEXPORT
#define __MCF_COND_INLINE  __MCF_DLLEXPORT
#include "cond.h"
#include "xglobals.h"

static __MCF_NEVER_INLINE
int
do_unlock_and_wait(_MCF_cond* cond, _MCF_cond_unlock_callback* unlock_opt, intptr_t* unlocked,
                   intptr_t lock_arg, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);
    _MCF_cond old, new;

    /* Allocate a count for the current thread.  */
    _MCF_atomic_load_pptr_rlx(&old, cond);
    for(;;) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__reserved_bits = 0;
      new.__nsleep = old.__nsleep + 1U;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_rlx(cond, &old, &new))
        break;
    }

    /* Now, unlock the associated mutex. If another thread attempts to signal
     * this one, it shall block.  */
    if(unlock_opt)
      *unlocked = (* unlock_opt) (lock_arg);

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(cond, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this condition variable
       * that an old waiter has left by decrementing the number of sleeping
       * threads. But see below...  */
      _MCF_atomic_load_pptr_rlx(&old, cond);
      for(;;) {
        if(old.__nsleep == 0)
          break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        new.__reserved_bits = 0;
        new.__nsleep = old.__nsleep - 1U;
#pragma GCC diagnostic pop

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(cond, &old, &new))
          return -1;
      }

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(cond, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    return 0;
  }

__MCF_DLLEXPORT
int
_MCF_cond_wait(_MCF_cond* cond, _MCF_cond_unlock_callback* unlock_opt, _MCF_cond_relock_callback* relock_opt,
               intptr_t lock_arg, const int64_t* timeout_opt)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    intptr_t unlocked;
    int err = do_unlock_and_wait(cond, unlock_opt, &unlocked, lock_arg, timeout_opt);

    /* If `relock_opt` is provided and the associated mutex has been unlocked,
     * relock it. Sometimes the mutex will be unlocked right after this wait
     * operation times out. As an optimization technique under such
     * circumstances, a user may pass a null `relock_opt` and do relocking
     * themself.  */
    if(unlock_opt && relock_opt)
      (* relock_opt) (lock_arg, unlocked);

    /* Forward the error code to caller.  */
    return err;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
size_t
_MCF_cond_signal_some_slow(_MCF_cond* cond, size_t max)
  {
    size_t wake_num;
    _MCF_cond old, new;

    /* Get the number of threads to wake up.  */
    _MCF_atomic_load_pptr_rlx(&old, cond);
    for(;;) {
      wake_num = _MCF_minz(old.__nsleep, max);
      if(wake_num == 0)
        return 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__reserved_bits = 0;
      new.__nsleep = old.__nsleep - wake_num;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_rlx(cond, &old, &new))
        break;
    }

    /* Wake up these threads.  */
    __MCF_batch_release_common(cond, wake_num);
    return wake_num;
  }
