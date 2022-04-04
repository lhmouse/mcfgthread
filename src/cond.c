// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_COND_C_  1
#include "cond.h"
#include "win32.h"

static void
do_wait_cleanup(_MCF_cond_unlock_callback* unlock_opt, intptr_t unlocked,
        _MCF_cond_relock_callback* relock_opt, intptr_t lock_arg)
  {
    if(unlock_opt && relock_opt)
      relock_opt(lock_arg, unlocked);
  }

bool
_MCF_cond_wait(_MCF_cond* cond, _MCF_cond_unlock_callback* unlock_opt,
        _MCF_cond_relock_callback* relock_opt, intptr_t lock_arg,
        const int64_t* timeout_opt)
  {
    // Allocate a count for the current thread.
    _MCF_cond new;
    _MCF_cond old;
    __atomic_load(cond, &old, __ATOMIC_ACQUIRE);

    do {
      new.__nsleep = (old.__nsleep + 1) & __MCF_COND_NS_M;
    }
    while(!__atomic_compare_exchange(cond, &old, &new,
                 true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

    // Now, invoke the unlock callback.
    // If another thread attempts to signal this one, it shall block.
    intptr_t unlocked = 42;
    if(unlock_opt)
      unlocked = unlock_opt(lock_arg);

    // Initialize the timeout value.
    LARGE_INTEGER timeout;
    bool use_timeout = false;

    if(timeout_opt) {
      if(*timeout_opt > 0) {
        // The timeout is the number of milliseconds since 1970-01-01T00:00:00Z.
        // Try converting it into the NT epoch.
        double delta = (double) *timeout_opt * 1e4 + 116444736e9;
        if(delta <= +9e18) {
          timeout.QuadPart = (int64_t) delta;
          use_timeout = true;
        }
      }
      else if(*timeout_opt < 0) {
        // The timeout is the number of milliseconds to wait.
        double delta = (double) *timeout_opt * 1e4;
        if(delta >= -9e18) {
          timeout.QuadPart = (int64_t) delta;
          use_timeout = true;
        }
      }
      else {
        // The operation shall time out immediately.
        timeout.QuadPart = 0;
        use_timeout = true;
      }
    }

    if(use_timeout) {
      for(;;) {
        // Try waiting.
        NTSTATUS status = NtWaitForKeyedEvent(NULL, cond, false, &timeout);
        if(!NT_SUCCESS(status))
          __MCF_PANIC();

        if(status != STATUS_TIMEOUT)
          break;

        // Tell another thread which is going to signal this condition variable
        // that an old waiter has left by decrementing the number of sleeping
        // threads. But see below...
        __atomic_load(cond, &old, __ATOMIC_ACQUIRE);

        do {
          if(old.__nsleep == 0)
            break;

          new.__nsleep = (old.__nsleep - 1) & __MCF_COND_NS_M;
        }
        while(!__atomic_compare_exchange(cond, &old, &new,
                     true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

        if(old.__nsleep != 0) {
          // The operation has timed out.
          do_wait_cleanup(unlock_opt, unlocked, relock_opt, lock_arg);
          return false;
        }

        // ... It is possible that the second thread has already decremented
        // the counter. If this does take place, the second thread is going to
        // release the keyed event soon. We must wait again, otherwise we get
        // a deadlock in the second thread. Again, a third thread could start
        // waiting for this keyed event before us, so we set the timeout to
        // zero. If we time out agian, the third thread will have incremented
        // the number of sleeping threads and we can try decrementing it again.
        timeout.QuadPart = 0;
      }
    }
    else {
      // The wait operation is infinite.
      NTSTATUS status = NtWaitForKeyedEvent(NULL, cond, false, NULL);
      if(!NT_SUCCESS(status))
        __MCF_PANIC();
    }

    // After the wakeup, relock the associated mutex, if any.
    do_wait_cleanup(unlock_opt, unlocked, relock_opt, lock_arg);
    return true;
  }

static size_t
do_signal_all_common(void* cond, size_t count)
  {
    // A call to `ExitProcess()` terminates all the other threads, even if they
    // are waiting for a condition variable. Don't release the keyed event in
    // this case, as it blocks the calling thread infinitely if there is no
    // thread to wake up.
    // See <https://github.com/lhmouse/mcfgthread/issues/21>.
    if(RtlDllShutdownInProgress())
      return 0;

    for(size_t k = 0;  k != count;  ++k) {
      // Release a thread. This operation shall block until the target thread
      // has received the notification.
      NTSTATUS status = NtReleaseKeyedEvent(NULL, cond, false, NULL);
      if(!NT_SUCCESS(status))
        __MCF_PANIC();
    }
    return count;
  }

size_t
_MCF_cond_signal_some(_MCF_cond* cond, size_t max)
  {
    // Get the number of threads to wake up.
    size_t nwoken;
    _MCF_cond new;
    _MCF_cond old;
    __atomic_load(cond, &old, __ATOMIC_ACQUIRE);

    do {
      nwoken = _MCF_minz(old.__nsleep, max);
      new.__nsleep = (old.__nsleep - nwoken) & __MCF_COND_NS_M;
    }
    while(!__atomic_compare_exchange(cond, &old, &new,
                 true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));

    return do_signal_all_common(cond, old.__nsleep);
  }

size_t
_MCF_cond_signal_all(_MCF_cond* cond)
  {
    // Swap out all data.
    _MCF_cond new = { 0 };
    _MCF_cond old;
    __atomic_exchange(cond, &new, &old, __ATOMIC_ACQ_REL);

    return do_signal_all_common(cond, old.__nsleep);
  }
