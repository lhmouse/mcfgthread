/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_ONCE_EXTERN_INLINE  __MCF_DLLEXPORT
#include "once.h"
#include "xwin32.i"

__MCF_DLLEXPORT
int
_MCF_once_wait_slow(_MCF_once* once, const int64_t* timeout_opt)
  {
    _MCF_once old, new;
    NTSTATUS status;
    LARGE_INTEGER timeout = __MCF_0_INIT;
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    int64_t waiting_since = 0;
    if(timeout_opt && (*timeout_opt < 0))
      waiting_since = (int64_t) GetTickCount64();

    for(;;) {
      /* If this flag has not been locked, lock it.
       * Otherwise, allocate a count for the current thread.  */
      _MCF_atomic_load_pptr_acq(&old, once);
      do {
        if(old.__ready != 0)
          return 0;

        new = old;
        if(old.__locked == 0)
          new.__locked = 1;
        else
          new.__nsleep = (old.__nsleep + 1U) & __MCF_ONCE_NSLEEP_M;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_arl(once, &old, &new));

      /* If this flag has been changed from UNLOCKED to LOCKED, return 1 to
       * allow initialization of protected resources.  */
      if(old.__locked == 0)
        return 1;

      /* Try waiting.  */
      status = __MCF_keyed_event_wait(once, use_timeout);
      if(!use_timeout)
        continue;

      while(status == STATUS_TIMEOUT) {
        /* Tell another thread which is going to signal this flag that an old
         * waiter has left by decrementing the number of sleeping threads. But
         * see below...  */
        _MCF_atomic_load_pptr_rlx(&old, once);
        do {
          if(old.__nsleep == 0)
            break;

          new = old;
          new.__nsleep = (old.__nsleep - 1U) & __MCF_ONCE_NSLEEP_M;
        }
        while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(once, &old, &new));

        /* We may still return something meaningful here.  */
        if(old.__nsleep != 0)
          return ((int) old.__ready - 1) >> 8;

        /* ... It is possible that a second thread has already decremented the
         * counter. If this does take place, it is going to release the keyed
         * event soon. We must wait again, otherwise we get a deadlock in the
         * second thread. Again, a third thread could start waiting for this
         * keyed event before us, so we set the timeout to zero. If we time out
         * again, the third thread will have incremented the number of sleeping
         * threads and we can try decrementing it again.  */
        status = __MCF_keyed_event_signal(once, (LARGE_INTEGER[1]) __MCF_0_INIT);
      }

      /* We have got notified. Recheck now.  */
      if(timeout.QuadPart >= 0)
        continue;

      /* If the timeout is relative, it has to be updated.  */
      int64_t now = (int64_t) GetTickCount64();
      timeout.QuadPart += (now - waiting_since) * 10000;
      waiting_since = now;
      if(timeout.QuadPart >= 0)
        return -1;
    }
  }

__MCF_DLLEXPORT
void
_MCF_once_abort(_MCF_once* once)
  {
    /* Clear the `__locked` field and release at most one thread, if any.  */
    size_t wake_one;
    _MCF_once old, new;

    _MCF_atomic_load_pptr_rlx(&old, once);
    do {
      new = old;
      new.__locked = 0;
      wake_one = _MCF_minz(old.__nsleep, 1);
      new.__nsleep = (old.__nsleep - wake_one) & __MCF_ONCE_NSLEEP_M;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(once, &old, &new));

    __MCF_batch_release_common(once, wake_one);
  }

__MCF_DLLEXPORT
void
_MCF_once_release(_MCF_once* once)
  {
    /* Set the `__ready` field and release all threads.  */
    _MCF_once old;
    _MCF_once new = { 1, 0, 0 };
    _MCF_atomic_xchg_pptr_arl(&old, once, &new);

    __MCF_batch_release_common(once, old.__nsleep);
  }
