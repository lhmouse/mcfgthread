/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_MUTEX_IMPORT  __MCF_DLLEXPORT
#define __MCF_MUTEX_INLINE  __MCF_DLLEXPORT
#include "mutex.h"
#include "xglobals.h"

static inline __MCF_FN_CONST
bool*
do_spin_byte_ptr(const _MCF_mutex* mutex, uint32_t sp_mask)
  {
    /* Each spinning thread is assigned a byte in the field. If the thread sees
     * this byte hold a value of zero, it continues spinning; otherwise, it
     * makes an attempt to lock the mutex where it is spinning. As the number
     * of spinning iterations is limited, this mechanism need not be reliable.  */
    static const uint32_t table_size = sizeof(__MCF_G(__mutex_spin_field));
    static const uint32_t table_size_reciprocal = 0x100000000U / table_size;

    /* We use an `uint32_t` as a fixed-point ratio within [0,1). Hence
     * `offset_in_table = ratio / 2^32 * table_size = ratio / (2^32 / table_size)`,
     * where `2^32 / table_size` is a constant.  */
    ULONG base, index;
    base = (uint32_t) ((uintptr_t) mutex / sizeof(void*)) * 0x9E3779B9U / table_size_reciprocal;
    __MCF_ASSERT(base < table_size);
    _BitScanForward(&index, sp_mask);
    __MCF_ASSERT(index < 4);
    return __MCF_G(__mutex_spin_field) + (base + index * (table_size / 4U)) % table_size;
  }

static inline __MCF_FN_CONST
uint32_t
do_adjust_sp_nfail(uint32_t old, int add)
  {
    /* Adjust the value using saturation arithmetic.  */
    __MCF_ASSERT((-1 <= add) && (add <= +1));
    uint32_t temp = old + (uint32_t) add;
    return temp - (temp >> 4);
  }

__MCF_DLLEXPORT
int
_MCF_mutex_lock_slow(_MCF_mutex* mutex, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout = __MCF_0_INIT;
    uint32_t sp_budget;
    _MCF_mutex old, new;

    /* Initialize the timeout value if a non-zero duration is specified. A
     * null pointer denotes infinity.  */
    if(!timeout_opt || (*timeout_opt != 0))
      __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, mutex);
    for(;;)
      if(old.__locked == 0) {
        /* The mutex is not locked, so lock it. The spinning failure counter
         * should be decremented, as this is the only chance that it could
         * ever be decremented after heavy contention.  */
        new.__locked = 1;
        new.__sp_mask = old.__sp_mask;
        new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, -1) & 0x0FU;
        new.__nsleep = old.__nsleep;

        if(_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new))
          return 0;
      }
      else if(nt_timeout.__li.QuadPart == 0) {
        /* The mutex is locked and we are not willing to wait, so fail.  */
        return -1;
      }
      else if((old.__sp_mask != 15U) && (old.__sp_nfail < __MCF_MUTEX_SP_NFAIL_THRESHOLD)) {
        /* The mutex is locked, but a spare spinning slot is available, so
         * allocate a slot and try spinning.  */
        sp_budget = __MCF_MUTEX_SP_NFAIL_THRESHOLD - old.__sp_nfail;
        new.__locked = 1;
        new.__sp_mask = (old.__sp_mask | (old.__sp_mask + 1U)) & 0x0FU;
        new.__sp_nfail = old.__sp_nfail;
        new.__nsleep = old.__nsleep;

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(mutex, &old, &new))
          break;
      }
      else {
        /* The mutex is locked, and no spare spinning slot is available, so
         * mark this operation as a spinning failure and allocate a sleeping
         * count.  */
        sp_budget = 0;
        new.__locked = 1;
        new.__sp_mask = old.__sp_mask;
        new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, +1) & 0x0FU;
        new.__nsleep = (old.__nsleep + 1U) & (__MCF_UPTR_MAX >> 9);

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(mutex, &old, &new))
          break;
      }

    /* Check whether the current thread is allowed to spin. Only one bit in
     * `new.__sp_mask` may have been set.  */
    if(sp_budget > 0) {
      uint32_t my_mask = (uint32_t) (old.__sp_mask ^ new.__sp_mask);
      __MCF_ASSERT(my_mask != 0);
      uint32_t sp_rem = sp_budget * (__MCF_MUTEX_MAX_SPIN_COUNT / __MCF_MUTEX_SP_NFAIL_THRESHOLD);
      while(sp_rem > 0) {
        sp_rem --;
        YieldProcessor();

        /* Wait for my turn. It's the simplest and fastest way to perform
         * just an atomic exchange, on both x86 and ARM.  */
        if(_MCF_atomic_xchg_b_rlx(do_spin_byte_ptr(mutex, my_mask), false) == false)
          continue;

        /* If this mutex has not been locked, lock it and decrement the
         * failure counter. Otherwise, do nothing.  */
        _MCF_atomic_load_pptr_rlx(&old, mutex);
        for(;;)
          if(old.__locked == 0) {
            new.__locked = 1;
            new.__sp_mask = old.__sp_mask & (15U - my_mask) & 0x0FU;
            new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, -1) & 0x0FU;
            new.__nsleep = old.__nsleep;

            if(_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new))
              return 0;
          }
          else {
            new.__locked = 1;
            new.__sp_mask = (old.__sp_mask | my_mask) & 0x0FU;
            new.__sp_nfail = old.__sp_nfail;
            new.__nsleep = old.__nsleep;

            if(_MCF_atomic_cmpxchg_weak_pptr_rlx(mutex, &old, &new))
              break;
          }
      }

      /* We have wasted some time, so return the spinning mask and allocate
       * a sleeping count.
       * IMPORTANT! We can increment the sleeping counter ONLY IF the mutex
       * is being locked by another thread. Otherwise, if the other thread
       * had unlocked the mutex before we incremented the sleeping counter,
       * we could miss a wakeup and result in deadlocks.  */
      _MCF_atomic_load_pptr_rlx(&old, mutex);
      for(;;)
        if(old.__locked == 0) {
          new.__locked = 1;
          new.__sp_mask = (old.__sp_mask & (15U - my_mask)) & 0x0FU;
          new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, -1) & 0x0FU;
          new.__nsleep = old.__nsleep;

          if(_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new))
            return 0;
        }
        else {
          new.__locked = 1;
          new.__sp_mask = (old.__sp_mask & (15U - my_mask)) & 0x0FU;
          new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, +1) & 0x0FU;
          new.__nsleep = (old.__nsleep + 1U) & (__MCF_UPTR_MAX >> 9);

          if(_MCF_atomic_cmpxchg_weak_pptr_rlx(mutex, &old, &new))
            break;
        }
    }

    /* Try waiting.  */
    int err = __MCF_keyed_event_wait(mutex, &nt_timeout);
    while(err != 0) {
      /* Tell another thread which is going to signal this mutex that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, mutex);
      for(;;) {
        if(old.__nsleep == 0)
          break;

        new.__locked = old.__locked;
        new.__sp_mask = old.__sp_mask;
        new.__sp_nfail = old.__sp_nfail;
        new.__nsleep = (old.__nsleep - 1U) & (__MCF_UPTR_MAX >> 9);

        if(_MCF_atomic_cmpxchg_weak_pptr_rlx(mutex, &old, &new))
          return -1;
      }

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      err = __MCF_keyed_event_wait(mutex, &(__MCF_winnt_timeout) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v3(&nt_timeout);
    goto try_lock_loop;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_mutex_unlock_slow(_MCF_mutex* mutex)
  {
    __MCF_ASSERT(mutex->__locked == 1);

    bool wake_one;
    _MCF_mutex old, new;

    /* Clear the `__locked` field and release at most one thread, if any.
     * The right most bit one of the spinning mask is also cleared to enable
     * further threads to spin.  */
    _MCF_atomic_load_pptr_rlx(&old, mutex);
    for(;;) {
      wake_one = old.__nsleep != 0;
      new.__locked = 0;
      new.__sp_mask = (old.__sp_mask & (old.__sp_mask - 1U)) & 0x0FU;
      new.__sp_nfail = old.__sp_nfail;
      new.__nsleep = (old.__nsleep - wake_one) & (__MCF_UPTR_MAX >> 9);

      if(_MCF_atomic_cmpxchg_weak_pptr_rel(mutex, &old, &new))
        break;
    }

    /* Notify a spinning thread, if any. If `__sp_mask` was non-zero, only its
     * rightmost bit should have been cleared, so we need not calculate the
     * bit difference, unlike `_MCF_mutex_lock_slow()`.  */
    if(old.__sp_mask != 0)
      _MCF_atomic_store_b_rlx(do_spin_byte_ptr(mutex, old.__sp_mask), true);

    /* Wake up a sleeping thread, if any.  */
    __MCF_batch_release_common(mutex, wake_one);
  }
