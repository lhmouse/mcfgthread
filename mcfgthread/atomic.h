/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_ATOMIC_
#define __MCFGTHREAD_ATOMIC_

#include "fwd.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_INLINE  __MCF_GNU_INLINE
#endif

#ifdef __GNUC__
/* Use GCC and Clang bultins which map to C++11 atomic functions directly. GCC
 * 4.7 or Clang 3.1 is required.  */
#  define __MCF__Atomic

#  define __MCF_atomic_succ_rlx   __ATOMIC_RELAXED
#  define __MCF_atomic_succ_acq   __ATOMIC_ACQUIRE
#  define __MCF_atomic_succ_rel   __ATOMIC_RELEASE
#  define __MCF_atomic_succ_arl   __ATOMIC_ACQ_REL
#  define __MCF_atomic_succ_cst   __ATOMIC_SEQ_CST

#  define __MCF_atomic_fail_rlx   __ATOMIC_RELAXED
#  define __MCF_atomic_fail_acq   __ATOMIC_ACQUIRE
#  define __MCF_atomic_fail_rel   __ATOMIC_RELAXED
#  define __MCF_atomic_fail_arl   __ATOMIC_ACQUIRE
#  define __MCF_atomic_fail_cst   __ATOMIC_SEQ_CST

#  define __MCF_atomic_load(p, ord)         __atomic_load_n(p, ord)
#  define __MCF_atomic_store(p, val, ord)   __atomic_store_n(p, val, ord)
#  define __MCF_atomic_xchg(p, val, ord)    __atomic_exchange_n(p, val, ord)
#  define __MCF_atomic_xadd(p, val, ord)    __atomic_fetch_add(p, val, ord)
#  define __MCF_atomic_xsub(p, val, ord)    __atomic_fetch_sub(p, val, ord)

#  define __MCF_atomic_cmpxchg(p, pcmp, val, ord_succ, ord_fail)  \
                __atomic_compare_exchange_n(p, pcmp, val, 0, ord_succ, ord_fail)

#  define __MCF_atomic_cmpxchg_weak(p, pcmp, val, ord_succ, ord_fail)  \
                __atomic_compare_exchange_n(p, pcmp, val, 1, ord_succ, ord_fail)
#else
/* Use C11 standard macros, provided by the system C library. In the case of
 * MSVC, Visual Studio 2022 is required.  */
#  include <stdatomic.h>
#  define __MCF__Atomic    __MCF_C11(_Atomic)

#  define __MCF_atomic_succ_rlx   memory_order_relaxed
#  define __MCF_atomic_succ_acq   memory_order_acquire
#  define __MCF_atomic_succ_rel   memory_order_release
#  define __MCF_atomic_succ_arl   memory_order_acq_rel
#  define __MCF_atomic_succ_cst   memory_order_seq_cst

#  define __MCF_atomic_fail_rlx   memory_order_relaxed
#  define __MCF_atomic_fail_acq   memory_order_acquire
#  define __MCF_atomic_fail_rel   memory_order_relaxed
#  define __MCF_atomic_fail_arl   memory_order_acquire
#  define __MCF_atomic_fail_cst   memory_order_seq_cst

#  define __MCF_atomic_load(p, ord)         atomic_load_explicit(p, ord)
#  define __MCF_atomic_store(p, val, ord)   atomic_store_explicit(p, val, ord)
#  define __MCF_atomic_xchg(p, val, ord)    atomic_exchange_explicit(p, val, ord)
#  define __MCF_atomic_xadd(p, val, ord)    atomic_fetch_add_explicit(p, val, ord)
#  define __MCF_atomic_xsub(p, val, ord)    atomic_fetch_sub_explicit(p, val, ord)

#  define __MCF_atomic_cmpxchg(p, pcmp, val, ord_succ, ord_fail)  \
           atomic_compare_exchange_strong_explicit(p, pcmp, val, ord_succ, ord_fail)

#  define __MCF_atomic_cmpxchg_weak(p, pcmp, val, ord_succ, ord_fail)  \
           atomic_compare_exchange_weak_explicit(p, pcmp, val, ord_succ, ord_fail)
#endif

#define __MCF_zp_r   const void* __restrict
#define __MCF_zp     void* __restrict

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
  _MCF_atomic_load_##WIDTH##_##ORDER(__MCF_zp_r __mem)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_load((const __MCF__Atomic INTEGER*) __mem,  \
                     __MCF_atomic_succ_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_load_p##WIDTH##_##ORDER(__MCF_zp __res, __MCF_zp_r __mem)  \
    __MCF_NOEXCEPT  \
    {  \
      *(INTEGER*) __res = __MCF_atomic_load((const __MCF__Atomic INTEGER*) __mem,  \
                                  __MCF_atomic_succ_##ORDER);  \
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
  _MCF_atomic_store_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val)  \
    __MCF_NOEXCEPT  \
    {  \
      __MCF_atomic_store((__MCF__Atomic INTEGER*) __mem, __val,  \
              __MCF_atomic_succ_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_store_p##WIDTH##_##ORDER(__MCF_zp __mem, __MCF_zp_r __src)  \
    __MCF_NOEXCEPT  \
    {  \
      __MCF_atomic_store((__MCF__Atomic INTEGER*) __mem, *(const INTEGER*) __src,  \
              __MCF_atomic_succ_##ORDER);  \
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
  _MCF_atomic_xchg_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_xchg((__MCF__Atomic INTEGER*) __mem, __val,  \
                     __MCF_atomic_succ_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_xchg_p##WIDTH##_##ORDER(__MCF_zp __res, __MCF_zp __mem, __MCF_zp_r __src)  \
    __MCF_NOEXCEPT  \
    {  \
      *(INTEGER*) __res = __MCF_atomic_xchg((__MCF__Atomic INTEGER*) __mem,  \
                                  *(const INTEGER*) __src, __MCF_atomic_succ_##ORDER);  \
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
 * functions may perform the operation as loops and will not fail spuriously.
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
  _MCF_atomic_cmpxchg_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER* __restrict __cmp, INTEGER __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchg((__MCF__Atomic INTEGER*) __mem, __cmp, __val,  \
                     __MCF_atomic_succ_##ORDER, __MCF_atomic_fail_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_p##WIDTH##_##ORDER(__MCF_zp __mem, __MCF_zp __cmp, __MCF_zp __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchg_weak((__MCF__Atomic INTEGER*) __mem, (INTEGER*) __cmp,  \
                 *(const INTEGER*) __val, __MCF_atomic_succ_##ORDER, __MCF_atomic_fail_##ORDER);  \
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
  _MCF_atomic_cmpxchg_weak_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER* __restrict __cmp, INTEGER __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchg_weak((__MCF__Atomic INTEGER*) __mem, __cmp, __val,  \
                     __MCF_atomic_succ_##ORDER, __MCF_atomic_fail_##ORDER);  \
    }  \
  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_weak_p##WIDTH##_##ORDER(__MCF_zp __mem, __MCF_zp __cmp, __MCF_zp __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_cmpxchg_weak((__MCF__Atomic INTEGER*) __mem, (INTEGER*) __cmp,  \
                 *(const INTEGER*) __val, __MCF_atomic_succ_##ORDER, __MCF_atomic_fail_##ORDER);  \
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
  _MCF_atomic_xadd_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_xadd((__MCF__Atomic INTEGER*) __mem, __val, __MCF_atomic_succ_##ORDER);  \
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

/* Subtract `val` from an atomic object. `mem` shall point to an atomic object of
 * the specified width. These function return the old value.
 *
 *   [INTEGER]
 *   _MCF_atomic_xsub_[WIDTH]_[ORDER](void* mem, [INTEGER] val);
 */
#define __MCF_atomic_xsub_(WIDTH, ORDER, INTEGER)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xsub_##WIDTH##_##ORDER(__MCF_zp __mem, INTEGER __val)  \
    __MCF_NOEXCEPT  \
    {  \
      return __MCF_atomic_xsub((__MCF__Atomic INTEGER*) __mem, __val, __MCF_atomic_succ_##ORDER);  \
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

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_ATOMIC_  */
