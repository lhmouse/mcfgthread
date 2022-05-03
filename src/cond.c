/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_COND_EXTERN_INLINE
#include "cond.h"
#include "win32.h"

struct cond_unlock_result
  {
    _MCF_cond_relock_callback* relock_opt;
    intptr_t lock_arg;
    intptr_t unlocked;
  };

static inline
void
do_unlock_cleanup(struct cond_unlock_result* res)
  {
    if(res->relock_opt)
      res->relock_opt(res->lock_arg, res->unlocked);
  }

int
_MCF_cond_wait(_MCF_cond* cond, _MCF_cond_unlock_callback* unlock_opt, _MCF_cond_relock_callback* relock_opt, intptr_t lock_arg, const int64_t* timeout_opt)
  {
    _MCF_cond old, new;
    NTSTATUS status;
    LARGE_INTEGER timeout = __MCF_0_INIT;
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);
    struct cond_unlock_result ulres __MCF_USE_DTOR(do_unlock_cleanup) = __MCF_0_INIT;

    /* Allocate a count for the current thread.  */
    __MCF_ATOMIC_LOAD_PTR_RLX(&old, cond);
    do {
      new = old;
      new.__nsleep = (old.__nsleep + 1U) & __MCF_COND_NSLEEP_M;
    }
    while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_RLX(cond, &old, &new));

    if(unlock_opt) {
      /* Now, unlock the associated mutex. If another thread attempts to signal
       * this one, it shall block.  */
      ulres.relock_opt = relock_opt;
      ulres.lock_arg = lock_arg;
      ulres.unlocked = unlock_opt(lock_arg);
    }

    /* Try waiting.  */
    status = NtWaitForKeyedEvent(NULL, cond, false, use_timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
    if(!use_timeout)
      return 0;

    while(status == STATUS_TIMEOUT) {
      /* Tell another thread which is going to signal this condition variable
       * that an old waiter has left by decrementing the number of sleeping
       * threads. But see below...  */
      __MCF_ATOMIC_LOAD_PTR_RLX(&old, cond);
      do {
        if(old.__nsleep == 0)
          break;

        new = old;
        new.__nsleep = (old.__nsleep - 1U) & __MCF_COND_NSLEEP_M;
      }
      while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_RLX(cond, &old, &new));

      if(old.__nsleep != 0)
        return -1;

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must wait again, otherwise we get a deadlock in the
       * second thread. Again, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * again, the third thread will have incremented the number of sleeping
       * threads and we can try decrementing it again.  */
      LARGE_INTEGER zero = __MCF_0_INIT;
      status = NtWaitForKeyedEvent(NULL, cond, false, &zero);
      __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
    }

    /* We have got notified.  */
    return 0;
  }

size_t
_MCF_cond_signal_some(_MCF_cond* cond, size_t max)
  {
    /* Get the number of threads to wake up.  */
    size_t wake_num;
    _MCF_cond old, new;

    __MCF_ATOMIC_LOAD_PTR_RLX(&old, cond);
    do {
      new = old;
      wake_num = _MCF_minz(old.__nsleep, max);
      new.__nsleep = (old.__nsleep - wake_num) & __MCF_COND_NSLEEP_M;
    }
    while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_RLX(cond, &old, &new));

    return __MCF_batch_release_common(cond, wake_num);
  }

size_t
_MCF_cond_signal_all(_MCF_cond* cond)
  {
    /* Swap out all data.  */
    _MCF_cond old;
    _MCF_cond new = __MCF_0_INIT;
    __MCF_ATOMIC_XCHG_PTR_RLX(&old, cond, &new);

    return __MCF_batch_release_common(cond, old.__nsleep);
  }
