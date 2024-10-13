/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_ONCE_IMPORT  __MCF_DLLEXPORT
#define __MCF_ONCE_INLINE  __MCF_DLLEXPORT
#include "once.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
_MCF_once_wait_slow(_MCF_once* once, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* If this flag has not been locked, lock it.
     * Otherwise, allocate a count for the current thread.  */
    _MCF_once old, new;
  try_lock_loop:
    _MCF_atomic_load_pptr_acq(&old, once);
#pragma GCC diagnostic ignored "-Wconversion"
    do {
      if(old.__ready != 0)
        return 0;

      new = old;
      new.__locked = 1;
      new.__nsleep = old.__nsleep + old.__locked;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_arl(once, &old, &new));
#pragma GCC diagnostic pop

    /* If this flag has been changed from UNLOCKED to LOCKED, return 1 to
     * allow initialization of protected resources.  */
    if(old.__locked == 0)
      return 1;

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(once, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this flag that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, once);
#pragma GCC diagnostic ignored "-Wconversion"
      do {
        if(old.__nsleep == 0)
          break;

        new = old;
        new.__nsleep = old.__nsleep - 1U;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(once, &old, &new));
#pragma GCC diagnostic pop

      /* We may still return something meaningful here.  */
      if(old.__nsleep != 0)
        return ((int) old.__ready - 1) >> 8;

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(once, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v3(&nt_timeout);
    goto try_lock_loop;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_once_abort(_MCF_once* once)
  {
    /* Clear the `__locked` field and release at most one thread, if any.  */
    size_t wake_one;
    _MCF_once old, new;
    _MCF_atomic_load_pptr_rlx(&old, once);
#pragma GCC diagnostic ignored "-Wconversion"
    do {
      new = old;
      new.__locked = 0;
      wake_one = _MCF_minz(old.__nsleep, 1);
      new.__nsleep = old.__nsleep - wake_one;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(once, &old, &new));
#pragma GCC diagnostic pop

    __MCF_batch_release_common(once, wake_one);
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_once_release(_MCF_once* once)
  {
    /* Set the `__ready` field and release all threads.  */
    _MCF_once old;
    _MCF_once new = { 1, 0, 0 };
    _MCF_atomic_xchg_pptr_arl(&old, once, &new);

    __MCF_batch_release_common(once, old.__nsleep);
  }
