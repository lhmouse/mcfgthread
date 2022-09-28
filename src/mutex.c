/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_MUTEX_IMPORT  __MCF_DLLEXPORT
#define __MCF_DECLSPEC_MUTEX_INLINE  __MCF_DLLEXPORT
#include "mutex.h"
#include "xglobals.i"

static inline
BYTE*
do_spin_byte_ptr(const _MCF_mutex* mutex, uint32_t sp_mask)
  {
    /* Each spinning thread is assigned a byte in the field. If the thread sees
     * this byte hold a value of zero, it continues spinning; otherwise, it
     * makes an attempt to lock the mutex where it is spinning. As the number
     * of spinning iterations is limited, this mechanism need not be reliable.  */
    static const uint32_t table_size = sizeof(__MCF_mutex_spin_field);
    static const uint32_t block_size = table_size / __builtin_ctz(__MCF_MUTEX_SP_MASK_M + 1U);
    static const uint32_t table_size_reciprocal = 0x100000000U / table_size;

    /* We use an `uint32_t` as a fixed-point ratio within [0,1). Hence
     * `offset_in_table = ratio / 2^32 * table_size = ratio / (2^32 /
     * table_size)`, where `2^32 / table_size` is a constant.  */
    uint32_t ratio = (uint32_t) ((uintptr_t) mutex / sizeof(void*)) * 0x9E3779B9U;
    DWORD base = ratio / table_size_reciprocal;
    __MCF_ASSERT(base < table_size);

    /* The unfortunate GCC `__builtin_ctz()` returns a signed integer which
     * results in terrible machine code, so we have to turn to something else
     * if its argument is not a constant.  */
    DWORD index;
    _BitScanForward(&index, sp_mask);

    return __MCF_mutex_spin_field + (base + index * block_size) % table_size;
  }

__MCF_DLLEXPORT
int
_MCF_mutex_lock_slow(_MCF_mutex* mutex, const int64_t* timeout_opt)
  {
    _MCF_mutex old, new;
    NTSTATUS status;

    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v2(&nt_timeout, timeout_opt);

    /* If this mutex has not been locked, lock it. Otherwise, if the maximum
     * number of spinning threads has been reached, allocate a sleeping count
     * for it. Otherwise, allocate a spinning bit for the current thread. If
     * the mutex can be locked immediately, the failure counter shall be
     * decremented. Otherwise it shall be incremented, no matter whether the
     * current thread is going to spin or not.  */
  try_lock_loop:
    _MCF_atomic_load_pptr_rlx(&old, mutex);
    do {
      new = old;
      if(old.__locked == 0)
        new.__locked = 1;
      else if((old.__sp_mask == __MCF_MUTEX_SP_MASK_M) || (old.__sp_nfail >= __MCF_MUTEX_SP_NFAIL_THRESHOLD))
        new.__nsleep = (old.__nsleep + 1U) & __MCF_MUTEX_NSLEEP_M;
      else
        new.__sp_mask = (old.__sp_mask | (old.__sp_mask + 1U)) & __MCF_MUTEX_SP_MASK_M;

      uint32_t temp = old.__sp_nfail - 1U + old.__locked * 2U;
      new.__sp_nfail = (temp - temp / (__MCF_MUTEX_SP_NFAIL_M + 1U)) & __MCF_MUTEX_SP_NFAIL_M;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_arl(mutex, &old, &new));

    /* If this mutex has been locked by the current thread, succeed.  */
    if(old.__locked == 0)
      return 0;

    if(old.__sp_mask != new.__sp_mask) {
      /* The current thread shall spin now.  */
      uint32_t my_mask = (uint32_t) new.__sp_mask ^ old.__sp_mask;
      __MCF_ASSERT((my_mask & (my_mask - 1)) == 0);

      /* Calculate the spin count for this loop.  */
      register int spin = (int) (__MCF_MUTEX_SP_NFAIL_THRESHOLD - old.__sp_nfail);
      __MCF_ASSERT(spin > 0);
      spin *= (int) (__MCF_MUTEX_MAX_SPIN_COUNT / __MCF_MUTEX_SP_NFAIL_THRESHOLD);

      while(--spin >= 0) {
        __builtin_ia32_pause();

        /* Wait for my turn.  */
        uint8_t cmp = 1;
        if(!_MCF_atomic_cmpxchg_weak_8_rlx(do_spin_byte_ptr(mutex, my_mask), &cmp, 0))
          continue;

        /* If this mutex has not been locked, lock it and decrement the
         * failure counter. Otherwise, do nothing.  */
        _MCF_atomic_load_pptr_rlx(&old, mutex);
        if(old.__locked != 0)
          continue;

        new = old;
        new.__locked = 1;

        uint32_t temp = old.__sp_nfail - 1U;
        new.__sp_nfail = (temp - temp / (__MCF_MUTEX_SP_NFAIL_M + 1U)) & __MCF_MUTEX_SP_NFAIL_M;

        if(_MCF_atomic_cmpxchg_pptr_acq(mutex, &old, &new))
          return 0;
      }

      /* We have wasted some time. Now allocate a sleeping count.
       * IMPORTANT! We can increment the sleeping counter ONLY IF the mutex
       * is being locked by another thread. Otherwise, if the other thread
       * had unlocked the mutex before we incremented the sleeping counter,
       * we could miss a wakeup and result in deadlocks.  */
      _MCF_atomic_load_pptr_rlx(&old, mutex);
      do {
        new = old;
        if(old.__locked == 0)
          new.__locked = 1;
        else
          new.__nsleep = (old.__nsleep + 1U) & __MCF_MUTEX_NSLEEP_M;

        uint32_t temp = old.__sp_nfail - 1U + old.__locked;
        new.__sp_nfail = (temp - temp / (__MCF_MUTEX_SP_NFAIL_M + 1U)) & __MCF_MUTEX_SP_NFAIL_M;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_acq(mutex, &old, &new));

      /* If this mutex has been locked by the current thread, succeed.  */
      if(old.__locked == 0)
        return 0;
    }

    /* Try waiting.  */
    status = __MCF_keyed_event_wait(mutex, nt_timeout.__li);
    while(status != STATUS_WAIT_0) {
      /* Tell another thread which is going to signal this mutex that an old
       * waiter has left by decrementing the number of sleeping threads. But
       * see below...  */
      _MCF_atomic_load_pptr_rlx(&old, mutex);
      do {
        if(old.__nsleep == 0)
          break;

        new = old;
        new.__nsleep = (old.__nsleep - 1U) & __MCF_MUTEX_NSLEEP_M;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(mutex, &old, &new));

      if(old.__nsleep != 0)
        return -1;

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      status = __MCF_keyed_event_signal(mutex, (LARGE_INTEGER[]) { 0 });
    }

    /* We have got notified.  */
    __MCF_adjust_winnt_timeout_v2(&nt_timeout);
    goto try_lock_loop;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
_MCF_mutex_unlock_slow(_MCF_mutex* mutex)
  {
    /* Clear the `__locked` field and release at most one thread, if any.
     * The right most bit one of the spinning mask is also cleared to enable
     * further threads to spin.  */
    size_t wake_one;
    _MCF_mutex old, new;

    _MCF_atomic_load_pptr_rlx(&old, mutex);
    do {
      new = old;
      new.__locked = 0;
      new.__sp_mask = (old.__sp_mask & (old.__sp_mask - 1U)) & __MCF_MUTEX_SP_MASK_M;
      wake_one = _MCF_minz(old.__nsleep, 1);
      new.__nsleep = (old.__nsleep - wake_one) & __MCF_MUTEX_NSLEEP_M;
    }
    while(!_MCF_atomic_cmpxchg_weak_pptr_rel(mutex, &old, &new));

    /* Notify a spinning thread, if any. If `__sp_mask` was non-zero, only its
     * rightmost bit should have been cleared, so we need not calculate the
     * bit difference, unlike `_MCF_mutex_lock_slow()`.  */
    if(old.__sp_mask != 0)
      _MCF_atomic_store_8_rlx(do_spin_byte_ptr(mutex, old.__sp_mask), 1);

    __MCF_batch_release_common(mutex, wake_one);
  }
