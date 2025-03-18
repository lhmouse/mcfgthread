/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
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
    static const uint32_t table_size = sizeof(__MCF_g->__mutex_spin_field);
    static const uint32_t table_size_reciprocal = 0x100000000U / table_size;

    /* We use an `uint32_t` as a fixed-point ratio within [0,1). Hence
     * `offset_in_table = ratio / 2^32 * table_size = ratio / (2^32 / table_size)`,
     * where `2^32 / table_size` is a constant.  */
    uint32_t ratio = (uint32_t) ((uintptr_t) mutex / sizeof(void*)) * 0x9E3779B9U;
    ULONG base = ratio / table_size_reciprocal;
    __MCF_ASSERT(base < table_size);

    /* The unfortunate GCC `__builtin_ctz()` returns a signed integer which
     * results in terrible machine code, so we have to turn to something else
     * if its argument is not a constant.  */
    ULONG index;
    _BitScanForward(&index, sp_mask);

    return __MCF_g->__mutex_spin_field + (base + index * (table_size / 4U)) % table_size;
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
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);
    int spin_count;
    _MCF_mutex old, new;

    /* If this mutex has not been locked, lock it; otherwise, if `__sp_mask`
     * is less than the number bits in `__sp_nfail` and `__sp_nfail` is less
     * than `__MCF_MUTEX_SP_NFAIL_THRESHOLD`, which means the current thread
     * is allowed to spin, allocate a spinning bit; otherwise, allocate a
     * sleeping count. The spinning failure counter is decremented if the
     * mutex can be locked immediately; and incremented otherwise, no matter
     * whether the current thread is going to spin or not.  */
  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, mutex);
    for(;;) {
      spin_count = (int) (__MCF_MUTEX_SP_NFAIL_THRESHOLD - old.__sp_nfail);
      bool may_spin = (old.__sp_mask != 15U) && (spin_count > 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic warning "-Wsign-conversion"
      new.__locked = 1;
      new.__sp_mask = old.__sp_mask | (old.__sp_mask + (old.__locked & may_spin));
      new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, (int) old.__locked * 2 - 1);
      new.__nsleep = old.__nsleep + (old.__locked & !may_spin);
#pragma GCC diagnostic pop

      if(_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new))
        break;
    }

    /* If this mutex has been locked by the current thread, succeed.  */
    if(old.__locked == 0)
      return 0;

    uint32_t lmask = (uint32_t) (old.__sp_mask ^ new.__sp_mask);
    if(lmask != 0) {
      __MCF_ASSERT(spin_count > 0);
      spin_count *= (int) (__MCF_MUTEX_MAX_SPIN_COUNT / __MCF_MUTEX_SP_NFAIL_THRESHOLD);

      while(__builtin_expect(spin_count, 100) > 0) {
        spin_count --;
        YieldProcessor();

        /* Wait for my turn. It's the simplest and fastest way to perform
         * just an atomic exchange, on both x86 and ARM.  */
        if(_MCF_atomic_xchg_b_rlx(do_spin_byte_ptr(mutex, lmask), false) == false)
          continue;

        /* If this mutex has not been locked, lock it and decrement the
         * failure counter. Otherwise, do nothing.  */
        _MCF_atomic_load_pptr_rlx(&old, mutex);
        for(;;) {
          if(old.__locked != 0)
            break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
          new.__locked = 1;
          new.__sp_mask = old.__sp_mask;
          new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, -1);
          new.__nsleep = old.__nsleep;
#pragma GCC diagnostic pop

          if(_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new))
            return 0;
        }
      }

      /* We have wasted some time, so return the spinning mask and allocate
       * a sleeping count.
       * IMPORTANT! We can increment the sleeping counter ONLY IF the mutex
       * is being locked by another thread. Otherwise, if the other thread
       * had unlocked the mutex before we incremented the sleeping counter,
       * we could miss a wakeup and result in deadlocks.  */
      _MCF_atomic_load_pptr_rlx(&old, mutex);
      for(;;) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        new.__locked = 1;
        new.__sp_mask = old.__sp_mask & ~lmask;
        new.__sp_nfail = do_adjust_sp_nfail(old.__sp_nfail, (int) old.__locked - 1);
        new.__nsleep = old.__nsleep + old.__locked;
#pragma GCC diagnostic pop

        if(_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new))
          break;
      }

      /* If this mutex has been locked by the current thread, succeed.  */
      if(old.__locked == 0)
        return 0;
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        new.__locked = old.__locked;
        new.__sp_mask = old.__sp_mask;
        new.__sp_nfail = old.__sp_nfail;
        new.__nsleep = old.__nsleep - 1U;
#pragma GCC diagnostic pop

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      new.__locked = 0;
      new.__sp_mask = old.__sp_mask & (old.__sp_mask - 1U);
      new.__sp_nfail = old.__sp_nfail;
      new.__nsleep = old.__nsleep - wake_one;
#pragma GCC diagnostic pop

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
