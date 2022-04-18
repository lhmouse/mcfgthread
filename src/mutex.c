/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#define __MCF_MUTEX_EXTERN_INLINE
#include "mutex.h"
#include "win32.h"

int
_MCF_mutex_lock_slow(_MCF_mutex* mutex, const int64_t* timeout_opt)
  {
    _MCF_mutex old, new;
    NTSTATUS status;
    LARGE_INTEGER timeout = { 0 };
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    int64_t waiting_since = 0;
    if(timeout_opt && (*timeout_opt < 0))
      waiting_since = (int64_t) GetTickCount64();

    for(;;) {
      /* If this mutex has not been locked, lock it.  */
      /* Otherwise, allocate a spinning count for the current thread. If the  */
      /* maximum number of spinning threads has been reached, allocate a  */
      /* sleeping count instead.  */
      __MCF_ATOMIC_LOAD_PTR_RLX(&old, mutex);
      do {
        new = old;
        if(old.__locked == 0)
          new.__locked = 1;
        else if((old.__nspin < __MCF_MUTEX_NSPIN_M) && (old.__nspin_fail < __MCF_MUTEX_SPIN_FAIL_THRESHOLD))
          new.__nspin = (old.__nspin + 1U) & __MCF_MUTEX_NSPIN_M;
        else
          new.__nsleep = (old.__nsleep + 1U) & __MCF_MUTEX_NSLEEP_M;

        /* If the mutex can be locked immediately, the failure counter shall be  */
        /* decremented. Otherwise it shall be incremented, no matter whether  */
        /* the current thread is going to spin or not.  */
        uint32_t temp = old.__nspin_fail + old.__locked * 2U - 1U;
        new.__nspin_fail = (temp - temp / (__MCF_MUTEX_NSPIN_FAIL_M + 1U)) & __MCF_MUTEX_NSPIN_FAIL_M;
      }
      while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_ARL(mutex, &old, &new));

      /* If this mutex has been locked by the current thread, succeed.  */
      if(old.__locked == 0)
        return 0;

      if(old.__nspin != new.__nspin) {
        /* Calculate the spin count for this loop.  */
        register int spin = (int) (__MCF_MUTEX_SPIN_FAIL_THRESHOLD - old.__nspin_fail);
        __MCFGTHREAD_ASSERT(spin > 0);
        spin *= (int) (__MCF_MUTEX_MAX_SPIN_COUNT / __MCF_MUTEX_SPIN_FAIL_THRESHOLD);

        while(--spin >= 0) {
          __builtin_ia32_pause();
          __atomic_thread_fence(__ATOMIC_SEQ_CST);

          /* If this mutex has not been locked, lock it, give back the spinning  */
          /* count, and decrement the failure counter. Otherwise, do nothing.  */
          __MCF_ATOMIC_LOAD_PTR_RLX(&old, mutex);
          if(old.__locked != 0)
            continue;

          new = old;
          new.__locked = 1;

          __MCFGTHREAD_ASSERT(old.__nspin != 0);
          new.__nspin = (old.__nspin - 1U) & __MCF_MUTEX_NSPIN_M;

          uint32_t temp = old.__nspin_fail - 1U;
          new.__nspin_fail = (temp - temp / (__MCF_MUTEX_NSPIN_FAIL_M + 1U)) & __MCF_MUTEX_NSPIN_FAIL_M;

          if(__MCF_ATOMIC_CMPXCHG_PTR_ACQ(mutex, &old, &new))
            return 0;
        }

        /* We have wasted some time. Now give back the spinning count and  */
        /* allocate a sleeping count.  */
        /* IMPORTANT! We can increment the sleeping counter ONLY IF the mutex  */
        /* is being locked by another thread. Otherwise, if the other thread  */
        /* had unlocked the mutex before we incremented the sleeping counter,  */
        /* we could miss a wakeup and result in deadlocks.  */
        __MCF_ATOMIC_LOAD_PTR_RLX(&old, mutex);
        do {
          new = old;
          if(old.__locked == 0)
            new.__locked = 1;
          else
            new.__nsleep = (old.__nsleep + 1U) & __MCF_MUTEX_NSLEEP_M;

          __MCFGTHREAD_ASSERT(old.__nspin != 0);
          new.__nspin = (old.__nspin - 1U) & __MCF_MUTEX_NSPIN_M;
        }
        while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_ACQ(mutex, &old, &new));

        /* If this mutex has been locked by the current thread, succeed.  */
        if(old.__locked == 0)
          return 0;
      }

      /* Try waiting.  */
      status = NtWaitForKeyedEvent(NULL, mutex, FALSE, use_timeout);
      __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
      if(!use_timeout) {
        /* The wait operation was infinite.  */
        continue;
      }

      while(status == STATUS_TIMEOUT) {
        /* Tell another thread which is going to signal this mutex that an old  */
        /* waiter has left by decrementing the number of sleeping threads. But  */
        /* see below...  */
        __MCF_ATOMIC_LOAD_PTR_RLX(&old, mutex);
        do {
          if(old.__nsleep == 0)
            break;

          new = old;
          new.__nsleep = (old.__nsleep - 1U) & __MCF_MUTEX_NSLEEP_M;
        }
        while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_RLX(mutex, &old, &new));

        if(old.__nsleep != 0) {
          /* The operation has timed out.  */
          return -1;
        }

        /* ... It is possible that a second thread has already decremented the  */
        /* counter. If this does take place, it is going to release the keyed  */
        /* event soon. We must wait again, otherwise we get a deadlock in the  */
        /* second thread. Again, a third thread could start waiting for this  */
        /* keyed event before us, so we set the timeout to zero. If we time out  */
        /* again, the third thread will have incremented the number of sleeping  */
        /* threads and we can try decrementing it again.  */
        LARGE_INTEGER zero = { 0 };
        status = NtWaitForKeyedEvent(NULL, mutex, FALSE, &zero);
        __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
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

void
_MCF_mutex_unlock(_MCF_mutex* mutex)
  {
    /* Clear the `__locked` field and release at most one thread, if any.  */
    size_t wake_one;
    _MCF_mutex old, new;

    __MCF_ATOMIC_LOAD_PTR_RLX(&old, mutex);
    do {
      new = old;
      new.__locked = 0;
      wake_one = _MCF_minz(old.__nsleep, 1);
      new.__nsleep = (old.__nsleep - wake_one) & __MCF_MUTEX_NSLEEP_M;
    }
    while(!__MCF_ATOMIC_CMPXCHG_WEAK_PTR_REL(mutex, &old, &new));

    __MCF_batch_release_common(mutex, wake_one);
  }
