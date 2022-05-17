/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_EVENT(...)  __MCF_DLLEXPORT
#include "event.h"
#include "xglobals.i"

int
_MCF_event_await_change_slow(_MCF_event* event, int undesired, const int64_t* timeout_opt)
  {
    _MCF_event old, new;
    NTSTATUS status;

    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v2(&nt_timeout, timeout_opt);

    for(;;) {
      /* If this event contains some other value, return immediately.
      *  Otherwise, allocate a count for the current thread.  */
      _MCF_atomic_load_pptr_acq(&old, event);
      do {
        if(old.__value != (uint8_t) undesired)
          return 0;

        new = old;
        new.__nsleep = (old.__nsleep + 1U) & __MCF_EVENT_NSLEEP_M;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_arl(event, &old, &new));

      /* Try waiting.  */
      status = __MCF_keyed_event_wait(event, nt_timeout.__li);
      while(status == STATUS_TIMEOUT) {
        /* Tell another thread which is going to signal this flag that an old
         * waiter has left by decrementing the number of sleeping threads. But
         * see below...  */
        _MCF_atomic_load_pptr_rlx(&old, event);
        do {
          if(old.__nsleep == 0)
            break;

          new = old;
          new.__nsleep = (old.__nsleep - 1U) & __MCF_EVENT_NSLEEP_M;
        }
        while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(event, &old, &new));

        if(old.__nsleep != 0)
          return -1;

        /* ... It is possible that a second thread has already decremented the
         * counter. If this does take place, it is going to release the keyed
         * event soon. We must wait again, otherwise we get a deadlock in the
         * second thread. Again, a third thread could start waiting for this
         * keyed event before us, so we set the timeout to zero. If we time out
         * again, the third thread will have incremented the number of sleeping
         * threads and we can try decrementing it again.  */
        status = __MCF_keyed_event_signal(event, (LARGE_INTEGER[1]) __MCF_0_INIT);
      }

      /* We have got notified. Recheck now.  */
      __MCF_adjust_winnt_timeout_v2(&nt_timeout);
    }
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
int
_MCF_event_set_slow(_MCF_event* event, int value)
  {
    if((value < 0) || (value > __MCF_EVENT_VALUE_MAX))
      return -1;

    /* Set the `__value` field and release all threads.  */
    _MCF_event old;
    _MCF_event new = { (uint8_t) value, 0 };
    _MCF_atomic_xchg_pptr_arl(&old, event, &new);

    __MCF_batch_release_common(event, old.__nsleep);
    return 0;
  }
