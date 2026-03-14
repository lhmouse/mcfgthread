/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_EVENT_IMPORT  __MCF_DLLEXPORT
#define __MCF_EVENT_INLINE  __MCF_DLLEXPORT
#include "event.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
_MCF_event_await_change_slow(_MCF_event* event, int undesired, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout = __MCF_0_INIT;
    _MCF_event old, new;

    /* Initialize the timeout value if a non-zero duration is specified. A
     * null pointer denotes infinity.  */
    if(!timeout_opt || (*timeout_opt != 0))
      __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* If this event contains some other value, return immediately.
    *  Otherwise, allocate a count for the current thread.  */
  try_wait_loop:
    _MCF_atomic_load_pptr_acq(&old, event);
    for(;;) {
      if(old.__value != (uint8_t) undesired)
        return old.__value;

      new.__value = old.__value;
      new.__reserved_bit = 0;
      new.__nsleep = (old.__nsleep + 1U) & (__MCF_UPTR_MAX >> 9);

      if(_MCF_atomic_cmpxchg_weak_pptr_arl(event, &old, &new))
        break;
    }

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(event, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this flag that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, event);
      for(;;) {
        if(old.__nsleep == 0)
          break;

        new.__value = old.__value;
        new.__reserved_bit = 0;
        new.__nsleep = (old.__nsleep - 1U) & (__MCF_UPTR_MAX >> 9);

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(event, &old, &new))
          return -1;
      }

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(event, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v3(&nt_timeout);
    goto try_wait_loop;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
int
_MCF_event_set_slow(_MCF_event* event, int value)
  {
    if((value < 0) || (value > __MCF_EVENT_VALUE_MAX))
      return -1;

    size_t wake_num;
    _MCF_event old, new;

    /* Get the number of threads to wake up.  */
    _MCF_atomic_load_pptr_rlx(&old, event);
    for(;;) {
      if(old.__value == (uint8_t) value)
        return 0;

      wake_num = old.__nsleep;
      new.__value = (uint8_t) value;
      new.__nsleep = 0;

      if(_MCF_atomic_cmpxchg_weak_pptr_rel(event, &old, &new))
        break;
    }

    /* Wake up these threads.  */
    __MCF_batch_release_common(event, wake_num);
    return 0;
  }
