/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_SHARED_MUTEX_IMPORT  __MCF_DLLEXPORT
#define __MCF_SHARED_MUTEX_INLINE  __MCF_DLLEXPORT
#include "shared_mutex.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
_MCF_shared_mutex_lock_shared_slow(_MCF_shared_mutex* smutex, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);
    _MCF_shared_mutex old, new;

    /* Grant shared access if and only if this shared mutex is either unlocked
     * (`__nshare` = 0), or in shared mode and the share counter won't overflow
     * (`__nshare` < `__MCF_SHARED_MUTEX_MAX_SHARE`). A reader shall not preempt
     * a lock. If shared access can't be granted, allocate a sleeping count.  */
  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, smutex);
    for(;;) {
      bool shareable = (old.__nshare < __MCF_SHARED_MUTEX_MAX_SHARE) && (old.__nsleep == 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__nshare = old.__nshare + shareable;
      new.__nsleep = old.__nsleep + !shareable;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_acq(smutex, &old, &new))
        break;
    }

    /* If this mutex has been locked by the current thread, succeed.  */
    if(old.__nshare != new.__nshare)
      return 0;

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(smutex, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this mutex that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, smutex);
      for(;;) {
        if(old.__nsleep == 0)
          break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        new.__nshare = old.__nshare;
        new.__nsleep = old.__nsleep - 1U;
#pragma GCC diagnostic pop

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(smutex, &old, &new))
          return -1;
      }

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(smutex, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v3(&nt_timeout);
    goto try_lock_loop;
  }

__MCF_DLLEXPORT
int
_MCF_shared_mutex_lock_exclusive_slow(_MCF_shared_mutex* smutex, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);
    _MCF_shared_mutex old, new;

    /* Grant exclusive access if and only if this shared mutex is unlocked
     * (`__nshare` = 0). A writer may preempt a lock, even when other threads
     * are waiting. If exclusive access can't be granted, allocate a sleeping
     * count.  */
  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, smutex);
    for(;;) {
      bool lockable = old.__nshare == 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__nshare = old.__nshare - lockable;
      new.__nsleep = old.__nsleep + !lockable;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_acq(smutex, &old, &new))
        break;
    }

    /* If this mutex has been locked by the current thread, succeed.  */
    if(old.__nshare == 0)
      return 0;

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(smutex, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this mutex that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, smutex);
      for(;;) {
        if(old.__nsleep == 0)
          break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        new.__nshare = old.__nshare;
        new.__nsleep = old.__nsleep - 1U;
#pragma GCC diagnostic pop

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(smutex, &old, &new))
          return -1;
      }

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(smutex, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v3(&nt_timeout);
    goto try_lock_loop;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_shared_mutex_unlock_slow(_MCF_shared_mutex* smutex)
  {
    __MCF_ASSERT(smutex->__nshare != 0);
    bool wake_one;
    _MCF_shared_mutex old, new;

    /* Determine whether the shared mutex has been locked in shared mode
     * (`__nshare` <= `__MCF_SHARED_MUTEX_MAX_SHARE`) or exclusive mode. The
     * last reader (`__nshare` = 1 before the call) or writer (`__nshare` =
     * `__MCF_SHARED_MUTEX_NSHARE_M` before the call) that unlocks a shared
     * mutex shall perform a wakeup operation.  */
    _MCF_atomic_load_pptr_rlx(&old, smutex);
    for(;;) {
      __MCF_ASSERT(old.__nshare != 0);
      bool exclusive = old.__nshare == 0x3FFF;
      wake_one = (old.__nsleep != 0) && !((old.__nshare > 1) && (old.__nshare < 0x3FFF));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__nshare = old.__nshare - !exclusive + exclusive;
      new.__nsleep = old.__nsleep - wake_one;
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_arl(smutex, &old, &new))
        break;
    }

    /* Wake up a sleeping thread, if any.  */
    __MCF_batch_release_common(smutex, wake_one);
  }
