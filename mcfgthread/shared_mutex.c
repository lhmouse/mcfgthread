/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_SHARED_MUTEX_IMPORT  __MCF_DLLEXPORT
#define __MCF_SHARED_MUTEX_INLINE  __MCF_DLLEXPORT
#include "shared_mutex.h"
#include "xglobals.h"

static
int
do_lock_common(_MCF_shared_mutex* smtx, bool shared, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout = __MCF_0_INIT;
    _MCF_shared_mutex old, new;

    /* Initialize the timeout value if a non-zero duration is specified. A
     * null pointer denotes infinity.  */
    if(!timeout_opt || (*timeout_opt != 0))
      __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, smtx);
    for(;;)
      if(shared && (old.__nshare < __MCF_SHARED_MUTEX_MAX_SHARE) && (old.__nsleep == 0)) {
        /* The mutex is either not locked or locked in shared mode, and no
         * thread is requesting exclusive access, so grant shared access.  */
        new.__nshare = (old.__nshare + 1U) & 0x3FFFU;
        new.__nsleep = old.__nsleep;

        if(_MCF_atomic_cmpxchg_weak_pptr_acq(smtx, &old, &new))
          return 0;
      }
      else if(!shared && old.__nshare == 0) {
        /* The mutex is not locked, so grant exclusive access.  */
        new.__nshare = 0x3FFFU;
        new.__nsleep = old.__nsleep;

        if(_MCF_atomic_cmpxchg_weak_pptr_acq(smtx, &old, &new))
          return 0;
      }
      else if(nt_timeout.__li.QuadPart == 0) {
        /* Access can't be granted, and we are not willing to wait, so fail.  */
        return -1;
      }
      else {
        /* Allocate a sleeping count for the current thread.  */
        new.__nshare = old.__nshare;
        new.__nsleep = (old.__nsleep + 1U) & (__MCF_UPTR_MAX >> 14);

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(smtx, &old, &new))
          break;
      }

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(smtx, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this mutex that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, smtx);
      for(;;) {
        if(old.__nsleep == 0)
          break;

        new.__nshare = old.__nshare;
        new.__nsleep = (old.__nsleep - 1U) & (__MCF_UPTR_MAX >> 14);

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(smtx, &old, &new))
          return -1;
      }

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(smtx, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v3(&nt_timeout);
    goto try_lock_loop;
  }

__MCF_DLLEXPORT
int
_MCF_shared_mutex_lock_shared_slow(_MCF_shared_mutex* smtx, const int64_t* timeout_opt)
  {
    return do_lock_common(smtx, true, timeout_opt);
  }

__MCF_DLLEXPORT
int
_MCF_shared_mutex_lock_exclusive_slow(_MCF_shared_mutex* smtx, const int64_t* timeout_opt)
  {
    return do_lock_common(smtx, false, timeout_opt);
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_shared_mutex_unlock_slow(_MCF_shared_mutex* smtx)
  {
    size_t wake_num;
    _MCF_shared_mutex old, new;

    /* Determine whether the shared mutex has been locked in shared mode
     * (`__nshare` <= `__MCF_SHARED_MUTEX_MAX_SHARE`) or exclusive mode. The
     * last reader (`__nshare` = 1 before the call) or writer (`__nshare` =
     * `0x3FFF` before the call) that unlocks a shared mutex shall perform a
     * wakeup operation. Since we can't know whether waiters are waiting for
     * shared or exclusive access, we have to wake up all threads.  */
    _MCF_atomic_load_pptr_rlx(&old, smtx);
    for(;;) {
      __MCF_ASSERT(old.__nshare != 0);

      uintptr_t new_nshare = (old.__nshare - 1U) & (((uint32_t) old.__nshare - 0x3FFFU) >> 14);
      wake_num = old.__nsleep & ((new_nshare - 1U) >> 14);
      new.__nshare = new_nshare & 0x3FFFU;
      new.__nsleep = (old.__nsleep - wake_num) & (__MCF_UPTR_MAX >> 14);

      if(_MCF_atomic_cmpxchg_weak_pptr_rel(smtx, &old, &new))
        break;
    }

    /* Wake up these threads.  */
    __MCF_batch_release_common(smtx, wake_num);
  }
