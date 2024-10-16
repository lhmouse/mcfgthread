/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_COND_IMPORT  __MCF_DLLEXPORT
#define __MCF_COND_INLINE  __MCF_DLLEXPORT
#include "cond.h"
#include "xglobals.h"

static
int
do_cond_wait_cleanup(_MCF_cond_relock_callback* relock_opt, intptr_t lock_arg, intptr_t unlocked, int err)
  {
    if(relock_opt)
      (*relock_opt) (lock_arg, unlocked);

    /* Forward the error code to caller.  */
    return err;
  }

__MCF_DLLEXPORT
int
_MCF_cond_wait(_MCF_cond* cond, _MCF_cond_unlock_callback* unlock_opt, _MCF_cond_relock_callback* relock_opt, intptr_t lock_arg, const int64_t* timeout_opt)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    _MCF_cond_relock_callback* relock_for_tail_call = __MCF_nullptr;
    intptr_t unlocked = 0;

    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* Allocate a count for the current thread.  */
    _MCF_cond old, new;
    _MCF_atomic_load_pptr_rlx(&old, cond);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    do {
      new.__reserved_bits = 0;
      new.__nsleep = old.__nsleep + 1U;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(cond, &old, &new));
#pragma GCC diagnostic pop

    if(unlock_opt) {
      /* Now, unlock the associated mutex. If another thread attempts to signal
       * this one, it shall block.  */
      relock_for_tail_call = relock_opt;
      unlocked = (*unlock_opt) (lock_arg);
    }

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(cond, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this condition variable
       * that an old waiter has left by decrementing the number of sleeping
       * threads. But see below...  */
      _MCF_atomic_load_pptr_rlx(&old, cond);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      while(old.__nsleep != 0) {
        new.__reserved_bits = 0;
        new.__nsleep = old.__nsleep - 1U;

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(cond, &old, &new))
          return do_cond_wait_cleanup(relock_for_tail_call, lock_arg, unlocked, -1);
      }
#pragma GCC diagnostic pop

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
    return do_cond_wait_cleanup(relock_for_tail_call, lock_arg, unlocked, 0);
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
size_t
_MCF_cond_signal_some_slow(_MCF_cond* cond, size_t max)
  {
    /* Get the number of threads to wake up.  */
    size_t wake_num;
    _MCF_cond old, new;
    _MCF_atomic_load_pptr_rlx(&old, cond);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    do {
      wake_num = _MCF_minz(old.__nsleep, max);
      new.__reserved_bits = 0;
      new.__nsleep = old.__nsleep - wake_num;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(cond, &old, &new));
#pragma GCC diagnostic pop

    return __MCF_batch_release_common(cond, wake_num);
  }
