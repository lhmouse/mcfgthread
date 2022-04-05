// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_ONCE_C_  1
#include "once.h"
#include "win32.h"

int
_MCF_once_wait_slow(_MCF_once* once, const int64_t* timeout_opt)
  {
    _MCF_once new, old;
    NTSTATUS status;

    if(timeout_opt && (*timeout_opt == 0)) {
      // If the timeout is zero, check whether it can be locked immediately.
      __atomic_load(once, &old, __ATOMIC_ACQUIRE);
      if(old.__ready != 0)
        return 0;

      if(old.__locked != 0)
        return -1;

      new = old;
      new.__locked = 1;
      if(!__atomic_compare_exchange(once, &old, &new,
                      FALSE, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
        return -1;

      // If this flag has been changed from UNLOCKED to LOCKED, return 1 to
      // allow initialization of protected resources.
      return 1;
    }

    LARGE_INTEGER timeout = { 0 };
    BOOLEAN use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    int64_t waiting_since = 0;
    if(timeout_opt && (*timeout_opt < 0))
      waiting_since = (int64_t) GetTickCount64();

    for(;;) {
      // If this flag has not been locked, lock it.
      // Otherwise, allocate a count for the current thread.
      __atomic_load(once, &old, __ATOMIC_RELAXED);
      do {
        if(old.__ready != 0)
          return 0;

        new = old;
        if(old.__locked == 0)
          new.__locked = 1;
        else
          new.__nsleep = (old.__nsleep + 1) & __MCF_ONCE_NS_M;
      }
      while(!__atomic_compare_exchange(once, &old, &new,
                        TRUE, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

      // If this flag has been changed from UNLOCKED to LOCKED, return 1 to
      // allow initialization of protected resources.
      if(old.__locked == 0)
        return 1;

      if(!use_timeout) {
        // The wait operation is infinite.
        status = NtWaitForKeyedEvent(NULL, once, FALSE, NULL);
        __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
        continue;
      }

      // Try waiting.
      status = NtWaitForKeyedEvent(NULL, once, FALSE, &timeout);
      __MCFGTHREAD_ASSERT(NT_SUCCESS(status));

      while(status == STATUS_TIMEOUT) {
        // Tell another thread which is going to signal this flat that an old
        // waiter has left by decrementing the number of sleeping threads. But
        // see below...
        __atomic_load(once, &old, __ATOMIC_RELAXED);
        do {
          if(old.__nsleep == 0)
            break;

          new = old;
          new.__nsleep = (old.__nsleep - 1) & __MCF_ONCE_NS_M;
        }
        while(!__atomic_compare_exchange(once, &old, &new,
                          TRUE, __ATOMIC_RELAXED, __ATOMIC_RELAXED));

        if(old.__nsleep != 0) {
          // The operation has timed out.
          // We may still return something meaningful here.
          return ((int) old.__ready - 1) >> 8;
        }

        // ... It is possible that a second thread has already decremented the
        // counter. If this does take place, it is going to release the keyed
        // event soon. We must wait again, otherwise we get a deadlock in the
        // second thread. Again, a third thread could start waiting for this
        // keyed event before us, so we set the timeout to zero. If we time out
        // agian, the third thread will have incremented the number of sleeping
        // threads and we can try decrementing it again.
        LARGE_INTEGER zero = { 0 };
        status = NtWaitForKeyedEvent(NULL, once, FALSE, &zero);
        __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
      }

      if(timeout.QuadPart < 0) {
        // If the timeout is relative, it has to be updated.
        int64_t now = (int64_t) GetTickCount64();
        timeout.QuadPart += (now - waiting_since) * 10000;
        waiting_since = now;

        if(timeout.QuadPart >= 0)
          return -1;
      }

      // We have got notified. Recheck now.
    }
  }

size_t
_MCF_once_abort(_MCF_once* once)
  {
    // Clear the `__locked` field and release at most one thread, if any.
    size_t wake_one;
    _MCF_once new, old;

    __atomic_load(once, &old, __ATOMIC_RELAXED);
    do {
      new = old;
      new.__locked = 0;
      wake_one = _MCF_minz(old.__nsleep, 1);
      new.__nsleep = (old.__nsleep - wake_one) & __MCF_ONCE_NS_M;
    }
    while(!__atomic_compare_exchange(once, &old, &new,
                      TRUE, __ATOMIC_RELAXED, __ATOMIC_RELAXED));

    return __MCF_batch_release_common(once, wake_one);
  }

size_t
_MCF_once_release(_MCF_once* once)
  {
    // Set the `__ready` field and release all threads.
    _MCF_once new = { 1, 0, 0 };
    _MCF_once old;
    __atomic_exchange(once, &new, &old, __ATOMIC_ACQ_REL);

    return __MCF_batch_release_common(once, old.__nsleep);
  }
