/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
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
    _MCF_once old, new;

    /* If this flag has not been locked, lock it.
     * Otherwise, allocate a count for the current thread.  */
  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, once);
    for(;;) {
      if(old.__ready != 0)
        return 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__ready = old.__ready;
      new.__locked = 1;
      new.__nsleep = old.__nsleep + old.__locked;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_acq(once, &old, &new))
        break;
    }

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
      for(;;) {
        if(old.__nsleep == 0)
          break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        new.__ready = old.__ready;
        new.__locked = old.__locked;
        new.__nsleep = old.__nsleep - 1U;
#pragma GCC diagnostic pop

        if(_MCF_atomic_cmpxchg_weak_pptr_acq(once, &old, &new))
          return (int) old.__ready - 1;
      }

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
    __MCF_ASSERT(once->__ready == 0);
    __MCF_ASSERT(once->__locked == 1);
    bool wake_one;
    _MCF_once old, new;

    /* Clear the `__locked` field and wake up a thread. This operation is
     * identical to a mutex.  */
    _MCF_atomic_load_pptr_rlx(&old, once);
    for(;;) {
      wake_one = old.__nsleep != 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__ready = old.__ready;
      new.__locked = 0;
      new.__nsleep = old.__nsleep - wake_one;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_rel(once, &old, &new))
        break;
    }

    /* Wake up a thread, if any.  */
    __MCF_batch_release_common(once, wake_one);
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_once_release(_MCF_once* once)
  {
    __MCF_ASSERT(once->__ready == 0);
    __MCF_ASSERT(once->__locked == 1);

    /* Set the `__ready` field and get the number of sleeping threads as an
     * atomic operation.  */
    _MCF_once old, new = { .__ready = 1 };
    _MCF_atomic_xchg_pptr_rel(&old, once, &new);

    /* Wake up all threads.  */
    __MCF_batch_release_common(once, old.__nsleep);
  }
