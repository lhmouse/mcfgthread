// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_ONCE_C_  1
#include "once.h"
#include "win32.h"

int
_MCF_once_wait_slow(_MCF_once* once, const int64_t* timeout_opt)
  {
    const double timeout_max = 0x7FFFFFFFFFFFFC00;
    LARGE_INTEGER timeout = { 0 };
    int64_t wait_start = 0;
    bool use_timeout = false;

    _MCF_once new;
    _MCF_once old;
    NTSTATUS status;

    // Initialize the timeout value.
    if(timeout_opt) {
      if(*timeout_opt > 0) {
        // The timeout is the number of milliseconds since 1970-01-01T00:00:00Z.
        // Try converting it into the NT epoch.
        double delta = (double) *timeout_opt * 10000 + 116444736000000000;
        if(delta <= timeout_max) {
          use_timeout = true;
          timeout.QuadPart = (int64_t) delta;
        }
      }
      else if(*timeout_opt < 0) {
        // The timeout is the number of milliseconds to wait.
        double delta = (double) *timeout_opt * 10000;
        if(delta >= -timeout_max) {
          use_timeout = true;
          timeout.QuadPart = (int64_t) delta;
          wait_start = (int64_t) GetTickCount64();
        }
      }
      else
        use_timeout = true;
    }

    if(use_timeout && (timeout.QuadPart == 0)) {
      // If the timeout is zero, check whether this flag can be locked only.
      __atomic_load(once, &old, __ATOMIC_ACQUIRE);
      if(old.__ready != 0)
        return 0;

      if(old.__locked != 0)
        return -1;

      new = old;
      new.__locked = 1;

      if(!__atomic_compare_exchange(once, &old, &new,
                      false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
        return -1;

      // If this flag has been changed from UNLOCKED to LOCKED, return 1 to
      // allow initialization of protected resources.
      return 1;
    }

    for(;;) {
      // If this flag has not been locked, lock it.
      // Otherwise, allocate a count for the current thread.
      __atomic_load(once, &old, __ATOMIC_ACQUIRE);
      do {
        if(old.__ready != 0)
          return 0;

        new = old;
        if(old.__locked != 0)
          new.__nsleep = (old.__nsleep + 1) & __MCF_ONCE_NS_M;
        else
          new.__locked = 1;
      }
      while(!__atomic_compare_exchange(once, &old, &new,
                   true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

      // If this flag has been changed from UNLOCKED to LOCKED, return 1 to
      // allow initialization of protected resources.
      if(old.__locked == 0)
        return 1;

      if(!use_timeout) {
        // The wait operation is infinite.
        status = NtWaitForKeyedEvent(NULL, once, false, NULL);
        if(!NT_SUCCESS(status))
          __MCF_PANIC();

        // Recheck now.
        continue;
      }

      // Try waiting.
      status = NtWaitForKeyedEvent(NULL, once, false, &timeout);
      if(!NT_SUCCESS(status))
        __MCF_PANIC();

      while(status == STATUS_TIMEOUT) {
        // Tell another thread which is going to signal this flat that an old
        // waiter has left by decrementing the number of sleeping threads. But
        // see below...
        __atomic_load(once, &old, __ATOMIC_ACQUIRE);
        do {
          if(old.__nsleep == 0)
            break;

          new = old;
          new.__nsleep = (old.__nsleep - 1) & __MCF_ONCE_NS_M;
        }
        while(!__atomic_compare_exchange(once, &old, &new,
                     true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

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
        status = NtWaitForKeyedEvent(NULL, once, false, &zero);
        if(!NT_SUCCESS(status))
          __MCF_PANIC();
      }

      if(timeout.QuadPart < 0) {
        // If the timeout is relative, it has to be updated.
        int64_t now = (int64_t) GetTickCount64();
        timeout.QuadPart += (now - wait_start) * 10000;
        wait_start = now;

        if(timeout.QuadPart >= 0)
          return -1;
      }

      // We have got notified. Recheck now.
    }
  }

static size_t
do_signal_all_common(void* once, size_t count)
  {
    if(count == 0)
      return 0;

    // A call to `ExitProcess()` terminates all the other threads, even if they
    // are waiting. Don't release the keyed event in this case, as it blocks
    // the calling thread infinitely if there is no thread to wake up.
    // See <https://github.com/lhmouse/mcfgthread/issues/21>.
    if(RtlDllShutdownInProgress())
      return 0;

    for(size_t k = 0;  k != count;  ++k) {
      // Release a thread. This operation shall block until the target thread
      // has received the notification.
      NTSTATUS status = NtReleaseKeyedEvent(NULL, once, false, NULL);
      if(!NT_SUCCESS(status))
        __MCF_PANIC();
    }
    return count;
  }

size_t
_MCF_once_abort(_MCF_once* once)
  {
    // Clear the `__locked` field and release at most one thread, if any.
    size_t wake_one;
    _MCF_once new;
    _MCF_once old;

    __atomic_load(once, &old, __ATOMIC_ACQUIRE);
    do {
      new = old;
      new.__locked = false;
      wake_one = _MCF_minz(old.__nsleep, 1);
      new.__nsleep = (old.__nsleep - wake_one) & __MCF_ONCE_NS_M;
    }
    while(!__atomic_compare_exchange(once, &old, &new,
                 true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

    return do_signal_all_common(once, wake_one);
  }

size_t
_MCF_once_release(_MCF_once* once)
  {
    // Set the `__ready` field and release all threads.
    _MCF_once new = { 1, 0, 0 };
    _MCF_once old;
    __atomic_exchange(once, &new, &old, __ATOMIC_ACQ_REL);

    return do_signal_all_common(once, old.__nsleep);
  }
