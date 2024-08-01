/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_ATOMIC_
#define __MCFGTHREAD_ATOMIC_

#include "fwd.h"

#if defined __GNUC__ || defined __clang__
/* Use native types.  */
#  define __MCF_atomic(...)   volatile __VA_ARGS__

#  define __MCF_memory_order_rlx     __ATOMIC_RELAXED
#  define __MCF_memory_order_acq     __ATOMIC_ACQUIRE
#  define __MCF_memory_order_rel     __ATOMIC_RELEASE
#  define __MCF_memory_order_arl     __ATOMIC_ACQ_REL
#  define __MCF_memory_order_cst     __ATOMIC_SEQ_CST

#  define __MCF_atomic_load(p,o)             __atomic_load_n(p,o)
#  define __MCF_atomic_store(p,v,o)          __atomic_store_n(p,v,o)
#  define __MCF_atomic_xchg(p,v,o)           __atomic_exchange_n(p,v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)    __atomic_compare_exchange_n(p,c,v,0,o,f)
#  define __MCF_atomic_cmpxchgw(p,c,v,o,f)   __atomic_compare_exchange_n(p,c,v,1,o,f)
#  define __MCF_atomic_xadd(p,v,o)           __atomic_fetch_add(p,v,o)
#  define __MCF_atomic_xsub(p,v,o)           __atomic_fetch_sub(p,v,o)
#  define __MCF_atomic_thread_fence(o)       __atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)       __atomic_signal_fence(o)
#endif

#if !defined __MCF_atomic && defined __cplusplus && (__cplusplus >= 201103L)
/* Use the C++11 standard library.  */
#  include <atomic>
#  define __MCF_atomic(...)   ::std::atomic<__VA_ARGS__>

#  define __MCF_memory_order_rlx     ::std::memory_order_relaxed
#  define __MCF_memory_order_acq     ::std::memory_order_acquire
#  define __MCF_memory_order_rel     ::std::memory_order_release
#  define __MCF_memory_order_arl     ::std::memory_order_acq_rel
#  define __MCF_memory_order_cst     ::std::memory_order_seq_cst

#  define __MCF_atomic_load(p,o)             (p)->load(o)
#  define __MCF_atomic_store(p,v,o)          (p)->store(v,o)
#  define __MCF_atomic_xchg(p,v,o)           (p)->exchange(v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)    (p)->compare_exchange_strong(*(c),v,o,f)
#  define __MCF_atomic_cmpxchgw(p,c,v,o,f)   (p)->compare_exchange_weak(*(c),v,o,f)
#  define __MCF_atomic_xadd(p,v,o)           (p)->fetch_add(v,o)
#  define __MCF_atomic_xsub(p,v,o)           (p)->fetch_sub(v,o)
#  define __MCF_atomic_thread_fence(o)       ::std::atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)       ::std::atomic_signal_fence(o)
#endif

#if !defined __MCF_atomic
/* Use the C11 standard library. Microsoft Visual Studio 2022 has experimental
 * support for this, but it seems to suffice.  */
#  include <stdatomic.h>
#  define __MCF_atomic(...)   _Atomic(__VA_ARGS__)

#  define __MCF_memory_order_rlx       memory_order_relaxed
#  define __MCF_memory_order_acq       memory_order_acquire
#  define __MCF_memory_order_rel       memory_order_release
#  define __MCF_memory_order_arl       memory_order_acq_rel
#  define __MCF_memory_order_cst       memory_order_seq_cst

#  define __MCF_atomic_load(p,o)             atomic_load_explicit(p,o)
#  define __MCF_atomic_store(p,v,o)          atomic_store_explicit(p,v,o)
#  define __MCF_atomic_xchg(p,v,o)           atomic_exchange_explicit(p,v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)    atomic_compare_exchange_strong_explicit(p,c,v,o,f)
#  define __MCF_atomic_cmpxchgw(p,c,v,o,f)   atomic_compare_exchange_weak_explicit(p,c,v,o,f)
#  define __MCF_atomic_xadd(p,v,o)           atomic_fetch_add_explicit(p,v,o)
#  define __MCF_atomic_xsub(p,v,o)           atomic_fetch_sub_explicit(p,v,o)
#  define __MCF_atomic_thread_fence(o)       atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)       atomic_signal_fence(o)
#endif

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_INLINE  __MCF_GNU_INLINE
#endif

/* Define memory orders for failed compare-and-swap operations.  */
#define __MCF_memory_order_f_rlx    __MCF_memory_order_rlx
#define __MCF_memory_order_f_acq    __MCF_memory_order_acq
#define __MCF_memory_order_f_rel    __MCF_memory_order_rlx
#define __MCF_memory_order_f_arl    __MCF_memory_order_acq
#define __MCF_memory_order_f_cst    __MCF_memory_order_cst

/* Perform an atomic load operation. `mem` shall point to an atomic object of the
 * specified width. The first function returns the value as an integer. The second
 * function writes the value into `*res`.
 *
 *   [INTEGER]
 *   _MCF_atomic_load_[WIDTH]_[ORDER](const void* mem);
 *
 *   void
 *   _MCF_atomic_load_p[WIDTH]_[ORDER](void* res, const void* mem);
 */
#define __MCF_atomic_load_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_load_##WIDTH##_##ORDER(__MCF_zp_r __mem) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_load((const __MCF_atomic(INTEGER)*) __mem,  \
                     __MCF_memory_order_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_load_p##WIDTH##_##ORDER(__MCF_zp __res, __MCF_zp_r __mem) __MCF_NOEXCEPT  \
    {  \
      *(INTEGER*) __res = __MCF_atomic_load((const __MCF_atomic(INTEGER)*) __mem,  \
                                  __MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_load_(8, rlx, int8_t)
__MCF_atomic_load_(8, acq, int8_t)
__MCF_atomic_load_(8, cst, int8_t)

__MCF_atomic_load_(16, rlx, int16_t)
__MCF_atomic_load_(16, acq, int16_t)
__MCF_atomic_load_(16, cst, int16_t)

__MCF_atomic_load_(32, rlx, int32_t)
__MCF_atomic_load_(32, acq, int32_t)
__MCF_atomic_load_(32, cst, int32_t)

__MCF_atomic_load_(64, rlx, int64_t)
__MCF_atomic_load_(64, acq, int64_t)
__MCF_atomic_load_(64, cst, int64_t)

__MCF_atomic_load_(ptr, rlx, intptr_t)
__MCF_atomic_load_(ptr, acq, intptr_t)
__MCF_atomic_load_(ptr, cst, intptr_t)

__MCF_atomic_load_(z, rlx, size_t)
__MCF_atomic_load_(z, acq, size_t)
__MCF_atomic_load_(z, cst, size_t)

/* Perform an atomic store operation. `mem` shall point to an atomic object of the
 * specified width. The first function stores the integer `val`. The second function
 * stores the value at `*src`.
 *
 *   void
 *   _MCF_atomic_store_[WIDTH]_[ORDER](void* mem, [INTEGER] val);
 *
 *   void
 *   _MCF_atomic_store_p[WIDTH]_[ORDER](void* mem, const void* src);
 */
#define __MCF_atomic_store_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_store_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      __MCF_atomic_store((__MCF_atomic(INTEGER)*) __mem, __val,  \
              __MCF_memory_order_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_store_p##WIDTH##_##ORDER(__MCF_zp __mem, __MCF_zp_r __src) __MCF_NOEXCEPT  \
    {  \
      __MCF_atomic_store((__MCF_atomic(INTEGER)*) __mem, *(const INTEGER*) __src,  \
              __MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_store_(8, rlx, int8_t)
__MCF_atomic_store_(8, rel, int8_t)
__MCF_atomic_store_(8, cst, int8_t)

__MCF_atomic_store_(16, rlx, int16_t)
__MCF_atomic_store_(16, rel, int16_t)
__MCF_atomic_store_(16, cst, int16_t)

__MCF_atomic_store_(32, rlx, int32_t)
__MCF_atomic_store_(32, rel, int32_t)
__MCF_atomic_store_(32, cst, int32_t)

__MCF_atomic_store_(64, rlx, int64_t)
__MCF_atomic_store_(64, rel, int64_t)
__MCF_atomic_store_(64, cst, int64_t)

__MCF_atomic_store_(ptr, rlx, intptr_t)
__MCF_atomic_store_(ptr, rel, intptr_t)
__MCF_atomic_store_(ptr, cst, intptr_t)

__MCF_atomic_store_(z, rlx, size_t)
__MCF_atomic_store_(z, rel, size_t)
__MCF_atomic_store_(z, cst, size_t)

/* Perform an atomic exchange operation. `mem` shall point to an atomic object of
 * the specified width. The first function stores the integer `val` and returns the
 * old value as an integer. The second function stores the value at `*src` and
 * writes the old value into `*res`.
 *
 *   [INTEGER]
 *   _MCF_atomic_xchg_[WIDTH]_[ORDER](void* mem, [INTEGER] val);
 *
 *   void
 *   _MCF_atomic_xchg_p[WIDTH]_[ORDER](void* res, void* mem, const void* src);
 */
#define __MCF_atomic_xchg_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xchg_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_xchg((__MCF_atomic(INTEGER)*) __mem, __val,  \
                     __MCF_memory_order_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_xchg_p##WIDTH##_##ORDER(__MCF_zp __res, __MCF_zp __mem, __MCF_zp_r __src) __MCF_NOEXCEPT  \
    {  \
      *(INTEGER*) __res = __MCF_atomic_xchg((__MCF_atomic(INTEGER)*) __mem,  \
                                  *(const INTEGER*) __src, __MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_xchg_(8, rlx, int8_t)
__MCF_atomic_xchg_(8, acq, int8_t)
__MCF_atomic_xchg_(8, rel, int8_t)
__MCF_atomic_xchg_(8, arl, int8_t)
__MCF_atomic_xchg_(8, cst, int8_t)

__MCF_atomic_xchg_(16, rlx, int16_t)
__MCF_atomic_xchg_(16, acq, int16_t)
__MCF_atomic_xchg_(16, rel, int16_t)
__MCF_atomic_xchg_(16, arl, int16_t)
__MCF_atomic_xchg_(16, cst, int16_t)

__MCF_atomic_xchg_(32, rlx, int32_t)
__MCF_atomic_xchg_(32, acq, int32_t)
__MCF_atomic_xchg_(32, rel, int32_t)
__MCF_atomic_xchg_(32, arl, int32_t)
__MCF_atomic_xchg_(32, cst, int32_t)

__MCF_atomic_xchg_(64, rlx, int64_t)
__MCF_atomic_xchg_(64, acq, int64_t)
__MCF_atomic_xchg_(64, rel, int64_t)
__MCF_atomic_xchg_(64, arl, int64_t)
__MCF_atomic_xchg_(64, cst, int64_t)

__MCF_atomic_xchg_(ptr, rlx, intptr_t)
__MCF_atomic_xchg_(ptr, acq, intptr_t)
__MCF_atomic_xchg_(ptr, rel, intptr_t)
__MCF_atomic_xchg_(ptr, arl, intptr_t)
__MCF_atomic_xchg_(ptr, cst, intptr_t)

__MCF_atomic_xchg_(z, rlx, size_t)
__MCF_atomic_xchg_(z, acq, size_t)
__MCF_atomic_xchg_(z, rel, size_t)
__MCF_atomic_xchg_(z, arl, size_t)
__MCF_atomic_xchg_(z, cst, size_t)

/* Perform a strong atomic compare-and-exchange operation. `mem` shall point to an
 * atomic object of the specified width. These functions compare the value of the
 * atomic object with `*cmp`. If they do not equal, these functions write the old
 * value into `*cmp` and return `false`. If they equal, these functions store the
 * integer `val` or the value at `*src` into `*mem`, and return `false`. These
 * functions may perform the operation as a loop and will not fail spuriously.
 *
 *   bool
 *   _MCF_atomic_cmpxchg_[WIDTH]_[ORDER](void* mem, [INTEGER]* cmp, [INTEGER] val);
 *
 *   bool
 *   _MCF_atomic_cmpxchg_p[WIDTH]_[ORDER](void* mem, void* cmp, const void* src);
 */
#define __MCF_atomic_cmpxchg_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER* __restrict __cmp, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchg((__MCF_atomic(INTEGER)*) __mem, __cmp,  \
                     __val, __MCF_memory_order_##ORDER,  \
                     __MCF_memory_order_f_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_p##WIDTH##_##ORDER(__MCF_zp __mem, __MCF_zp __cmp, __MCF_zp __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchg((__MCF_atomic(INTEGER)*) __mem, (INTEGER*) __cmp,  \
                 *(const INTEGER*) __val, __MCF_memory_order_##ORDER,  \
                 __MCF_memory_order_f_##ORDER);  \
    }

__MCF_atomic_cmpxchg_(8, rlx, int8_t)
__MCF_atomic_cmpxchg_(8, acq, int8_t)
__MCF_atomic_cmpxchg_(8, rel, int8_t)
__MCF_atomic_cmpxchg_(8, arl, int8_t)
__MCF_atomic_cmpxchg_(8, cst, int8_t)

__MCF_atomic_cmpxchg_(16, rlx, int16_t)
__MCF_atomic_cmpxchg_(16, acq, int16_t)
__MCF_atomic_cmpxchg_(16, rel, int16_t)
__MCF_atomic_cmpxchg_(16, arl, int16_t)
__MCF_atomic_cmpxchg_(16, cst, int16_t)

__MCF_atomic_cmpxchg_(32, rlx, int32_t)
__MCF_atomic_cmpxchg_(32, acq, int32_t)
__MCF_atomic_cmpxchg_(32, rel, int32_t)
__MCF_atomic_cmpxchg_(32, arl, int32_t)
__MCF_atomic_cmpxchg_(32, cst, int32_t)

__MCF_atomic_cmpxchg_(64, rlx, int64_t)
__MCF_atomic_cmpxchg_(64, acq, int64_t)
__MCF_atomic_cmpxchg_(64, rel, int64_t)
__MCF_atomic_cmpxchg_(64, arl, int64_t)
__MCF_atomic_cmpxchg_(64, cst, int64_t)

__MCF_atomic_cmpxchg_(ptr, rlx, intptr_t)
__MCF_atomic_cmpxchg_(ptr, acq, intptr_t)
__MCF_atomic_cmpxchg_(ptr, rel, intptr_t)
__MCF_atomic_cmpxchg_(ptr, arl, intptr_t)
__MCF_atomic_cmpxchg_(ptr, cst, intptr_t)

__MCF_atomic_cmpxchg_(z, rlx, size_t)
__MCF_atomic_cmpxchg_(z, acq, size_t)
__MCF_atomic_cmpxchg_(z, rel, size_t)
__MCF_atomic_cmpxchg_(z, arl, size_t)
__MCF_atomic_cmpxchg_(z, cst, size_t)

/* Perform a weak atomic compare-and-exchange operation. `mem` shall point to an
 * atomic object of the specified width. These functions compare the value of the
 * atomic object with `*cmp`. If they do not equal, these functions write the old
 * value into `*cmp` and return `false`. If they equal, these functions store the
 * integer `val` or the value at `*src` into `*mem`, and return `false`. These
 * functions may fail spuriously.
 *
 *   bool
 *   _MCF_atomic_cmpxchg_weak_[WIDTH]_[ORDER](void* mem, [INTEGER]* cmp, [INTEGER] val);
 *
 *   bool
 *   _MCF_atomic_cmpxchg_weak_p[WIDTH]_[ORDER](void* mem, void* cmp, const void* src);
 */
#define __MCF_atomic_cmpxchg_weak_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_weak_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER* __restrict __cmp, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchgw((__MCF_atomic(INTEGER)*) __mem, __cmp,  \
                     __val, __MCF_memory_order_##ORDER,  \
                     __MCF_memory_order_f_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_weak_p##WIDTH##_##ORDER(__MCF_zp __mem, __MCF_zp __cmp, __MCF_zp __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchgw((__MCF_atomic(INTEGER)*) __mem, (INTEGER*) __cmp,  \
                     *(const INTEGER*) __val, __MCF_memory_order_##ORDER,  \
                     __MCF_memory_order_f_##ORDER);  \
    }

__MCF_atomic_cmpxchg_weak_(8, rlx, int8_t)
__MCF_atomic_cmpxchg_weak_(8, acq, int8_t)
__MCF_atomic_cmpxchg_weak_(8, rel, int8_t)
__MCF_atomic_cmpxchg_weak_(8, arl, int8_t)
__MCF_atomic_cmpxchg_weak_(8, cst, int8_t)

__MCF_atomic_cmpxchg_weak_(16, rlx, int16_t)
__MCF_atomic_cmpxchg_weak_(16, acq, int16_t)
__MCF_atomic_cmpxchg_weak_(16, rel, int16_t)
__MCF_atomic_cmpxchg_weak_(16, arl, int16_t)
__MCF_atomic_cmpxchg_weak_(16, cst, int16_t)

__MCF_atomic_cmpxchg_weak_(32, rlx, int32_t)
__MCF_atomic_cmpxchg_weak_(32, acq, int32_t)
__MCF_atomic_cmpxchg_weak_(32, rel, int32_t)
__MCF_atomic_cmpxchg_weak_(32, arl, int32_t)
__MCF_atomic_cmpxchg_weak_(32, cst, int32_t)

__MCF_atomic_cmpxchg_weak_(64, rlx, int64_t)
__MCF_atomic_cmpxchg_weak_(64, acq, int64_t)
__MCF_atomic_cmpxchg_weak_(64, rel, int64_t)
__MCF_atomic_cmpxchg_weak_(64, arl, int64_t)
__MCF_atomic_cmpxchg_weak_(64, cst, int64_t)

__MCF_atomic_cmpxchg_weak_(ptr, rlx, intptr_t)
__MCF_atomic_cmpxchg_weak_(ptr, acq, intptr_t)
__MCF_atomic_cmpxchg_weak_(ptr, rel, intptr_t)
__MCF_atomic_cmpxchg_weak_(ptr, arl, intptr_t)
__MCF_atomic_cmpxchg_weak_(ptr, cst, intptr_t)

__MCF_atomic_cmpxchg_weak_(z, rlx, size_t)
__MCF_atomic_cmpxchg_weak_(z, acq, size_t)
__MCF_atomic_cmpxchg_weak_(z, rel, size_t)
__MCF_atomic_cmpxchg_weak_(z, arl, size_t)
__MCF_atomic_cmpxchg_weak_(z, cst, size_t)

/* Add `val` to an atomic object. `mem` shall point to an atomic object of the
 * specified width. These function return the old value.
 *
 *   [INTEGER]
 *   _MCF_atomic_xadd_[WIDTH]_[ORDER](void* mem, [INTEGER] val);
 */
#define __MCF_atomic_xadd_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xadd_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_xadd((__MCF_atomic(INTEGER)*) __mem, __val,  \
                     __MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_xadd_(8, rlx, int8_t)
__MCF_atomic_xadd_(8, acq, int8_t)
__MCF_atomic_xadd_(8, rel, int8_t)
__MCF_atomic_xadd_(8, arl, int8_t)
__MCF_atomic_xadd_(8, cst, int8_t)

__MCF_atomic_xadd_(16, rlx, int16_t)
__MCF_atomic_xadd_(16, acq, int16_t)
__MCF_atomic_xadd_(16, rel, int16_t)
__MCF_atomic_xadd_(16, arl, int16_t)
__MCF_atomic_xadd_(16, cst, int16_t)

__MCF_atomic_xadd_(32, rlx, int32_t)
__MCF_atomic_xadd_(32, acq, int32_t)
__MCF_atomic_xadd_(32, rel, int32_t)
__MCF_atomic_xadd_(32, arl, int32_t)
__MCF_atomic_xadd_(32, cst, int32_t)

__MCF_atomic_xadd_(64, rlx, int64_t)
__MCF_atomic_xadd_(64, acq, int64_t)
__MCF_atomic_xadd_(64, rel, int64_t)
__MCF_atomic_xadd_(64, arl, int64_t)
__MCF_atomic_xadd_(64, cst, int64_t)

__MCF_atomic_xadd_(ptr, rlx, intptr_t)
__MCF_atomic_xadd_(ptr, acq, intptr_t)
__MCF_atomic_xadd_(ptr, rel, intptr_t)
__MCF_atomic_xadd_(ptr, arl, intptr_t)
__MCF_atomic_xadd_(ptr, cst, intptr_t)

__MCF_atomic_xadd_(z, rlx, size_t)
__MCF_atomic_xadd_(z, acq, size_t)
__MCF_atomic_xadd_(z, rel, size_t)
__MCF_atomic_xadd_(z, arl, size_t)
__MCF_atomic_xadd_(z, cst, size_t)

/* Subtract `val` from an atomic object. `mem` shall point to an atomic object
 * of the specified width. These function return the old value.
 *
 *   [INTEGER]
 *   _MCF_atomic_xsub_[WIDTH]_[ORDER](void* mem, [INTEGER] val);
 */
#define __MCF_atomic_xsub_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xsub_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_xsub((__MCF_atomic(INTEGER)*) __mem, __val,  \
                     __MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_xsub_(8, rlx, int8_t)
__MCF_atomic_xsub_(8, acq, int8_t)
__MCF_atomic_xsub_(8, rel, int8_t)
__MCF_atomic_xsub_(8, arl, int8_t)
__MCF_atomic_xsub_(8, cst, int8_t)

__MCF_atomic_xsub_(16, rlx, int16_t)
__MCF_atomic_xsub_(16, acq, int16_t)
__MCF_atomic_xsub_(16, rel, int16_t)
__MCF_atomic_xsub_(16, arl, int16_t)
__MCF_atomic_xsub_(16, cst, int16_t)

__MCF_atomic_xsub_(32, rlx, int32_t)
__MCF_atomic_xsub_(32, acq, int32_t)
__MCF_atomic_xsub_(32, rel, int32_t)
__MCF_atomic_xsub_(32, arl, int32_t)
__MCF_atomic_xsub_(32, cst, int32_t)

__MCF_atomic_xsub_(64, rlx, int64_t)
__MCF_atomic_xsub_(64, acq, int64_t)
__MCF_atomic_xsub_(64, rel, int64_t)
__MCF_atomic_xsub_(64, arl, int64_t)
__MCF_atomic_xsub_(64, cst, int64_t)

__MCF_atomic_xsub_(ptr, rlx, intptr_t)
__MCF_atomic_xsub_(ptr, acq, intptr_t)
__MCF_atomic_xsub_(ptr, rel, intptr_t)
__MCF_atomic_xsub_(ptr, arl, intptr_t)
__MCF_atomic_xsub_(ptr, cst, intptr_t)

__MCF_atomic_xsub_(z, rlx, size_t)
__MCF_atomic_xsub_(z, acq, size_t)
__MCF_atomic_xsub_(z, rel, size_t)
__MCF_atomic_xsub_(z, arl, size_t)
__MCF_atomic_xsub_(z, cst, size_t)

/* Emit a fence between threads.
 *
 *   void
 *   _MCF_thread_fence_[ORDER]();
 */
#define __MCF_atomic_thread_fence_(ORDER)  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_thread_fence_##ORDER(void) __MCF_NOEXCEPT  \
    {  \
      __MCF_atomic_thread_fence(__MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_thread_fence_(rlx)
__MCF_atomic_thread_fence_(acq)
__MCF_atomic_thread_fence_(rel)
__MCF_atomic_thread_fence_(arl)
__MCF_atomic_thread_fence_(cst)

/* Emit a fence within the same thread.
 *
 *   void
 *   _MCF_signal_fence_[ORDER]();
 */
#define __MCF_atomic_signal_fence_(ORDER)  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_signal_fence_##ORDER(void) __MCF_NOEXCEPT  \
    {  \
      __MCF_atomic_signal_fence(__MCF_memory_order_##ORDER);  \
    }

__MCF_atomic_signal_fence_(rlx)
__MCF_atomic_signal_fence_(acq)
__MCF_atomic_signal_fence_(rel)
__MCF_atomic_signal_fence_(arl)
__MCF_atomic_signal_fence_(cst)

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_ATOMIC_  */
