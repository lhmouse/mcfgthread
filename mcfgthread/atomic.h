/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_ATOMIC_
#define __MCFGTHREAD_ATOMIC_

#include "fwd.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_INLINE  __MCF_GNU_INLINE
#endif

/* We don't use the generic builtins due to a GCC bug.
 * See <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105495> for details.
 *
 * These functions are provided as inlined ones only. Hereinafter, `WIDTH` sets
 * `INTEGER` to `intWIDTH_t`. `ORDER` may be `rlx` for relaxed order, `acq` for
 * acquire order, `rel` for release order, `arl` for both acquire and release
 * order, or `cst` for sequentially-consistent order.
 *
 * All memory orders are defined below.  */
#define __MCF_ATOMIC_SUCC_rlx   __ATOMIC_RELAXED
#define __MCF_ATOMIC_SUCC_acq   __ATOMIC_ACQUIRE
#define __MCF_ATOMIC_SUCC_rel   __ATOMIC_RELEASE
#define __MCF_ATOMIC_SUCC_arl   __ATOMIC_ACQ_REL
#define __MCF_ATOMIC_SUCC_cst   __ATOMIC_SEQ_CST

#define __MCF_ATOMIC_FAIL_rlx   __ATOMIC_RELAXED
#define __MCF_ATOMIC_FAIL_acq   __ATOMIC_ACQUIRE
#define __MCF_ATOMIC_FAIL_rel   __ATOMIC_RELAXED
#define __MCF_ATOMIC_FAIL_arl   __ATOMIC_ACQUIRE
#define __MCF_ATOMIC_FAIL_cst   __ATOMIC_SEQ_CST

#define __MCF_ATOMIC_FUNCTIONS(function, o)  \
  function(   int8_t,   8_##o, __MCF_ATOMIC_SUCC_##o, __MCF_ATOMIC_FAIL_##o)  \
  function(  int16_t,  16_##o, __MCF_ATOMIC_SUCC_##o, __MCF_ATOMIC_FAIL_##o)  \
  function(  int32_t,  32_##o, __MCF_ATOMIC_SUCC_##o, __MCF_ATOMIC_FAIL_##o)  \
  function(  int64_t,  64_##o, __MCF_ATOMIC_SUCC_##o, __MCF_ATOMIC_FAIL_##o)  \
  function( intptr_t, ptr_##o, __MCF_ATOMIC_SUCC_##o, __MCF_ATOMIC_FAIL_##o)  \
  function(   size_t,   z_##o, __MCF_ATOMIC_SUCC_##o, __MCF_ATOMIC_FAIL_##o)

/* Performs a scalar atomic load operation.
 * The value that has been read is returned as an integer.
 *
 * `INTEGER _MCF_atomic_load_WIDTH_ORDER(const void* mem);`  */
#define __MCF_atomic_load_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_load_##SUFFIX(const void* __mem) __MCF_NOEXCEPT  \
    {  \
      return __atomic_load_n((const INTEGER*) __mem, ORDER_SUCC);  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_load_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_load_, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_load_, cst)

/* Performs a generic atomic load operation.
 * The value that has been read is returned via the pointer `retp`.
 *
 * `void _MCF_atomic_load_pWIDTH_ORDER(void* retp, const void* mem);`  */
#define __MCF_atomic_load_p(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_load_p##SUFFIX(void* __retp, const void* __mem) __MCF_NOEXCEPT  \
    {  \
      INTEGER __val = __atomic_load_n((const INTEGER*) __mem, ORDER_SUCC);  \
      *(INTEGER*) __retp = __val;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_load_p, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_load_p, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_load_p, cst)

/* Performs a scalar atomic store operation.
 * The value to write is passed by value as an integer.
 *
 * `INTEGER _MCF_atomic_store_WIDTH_ORDER(void* mem, INTEGER val);`  */
#define __MCF_atomic_store_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_store_##SUFFIX(void* __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      __atomic_store_n((INTEGER*) __mem, __val, ORDER_SUCC);  \
      return __val;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_store_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_store_, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_store_, cst)

/* Performs a generic atomic store operation.
 * The value to write is passed via the pointer `valp`.
 *
 * `void _MCF_atomic_store_pWIDTH_ORDER(void* mem, const void* valp);`  */
#define __MCF_atomic_store_p(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_store_p##SUFFIX(void* __mem, const void* __valp) __MCF_NOEXCEPT  \
    {  \
      INTEGER __val = *(const INTEGER*) __valp;  \
      __atomic_store_n((INTEGER*) __mem, __val, ORDER_SUCC);  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_store_p, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_store_p, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_store_p, cst)

/* Performs a scalar atomic exchange operation.
 * The previous value is returned as an integer.
 * The value to write is passed by value as an integer.
 *
 * `INTEGER _MCF_atomic_xchg_WIDTH_ORDER(void* mem, INTEGER val);`  */
#define __MCF_atomic_xchg_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xchg_##SUFFIX(void* __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __atomic_exchange_n((INTEGER*) __mem, __val, ORDER_SUCC);  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_, cst)

/* Performs a generic atomic exchange operation.
 * The previous value is returned via the pointer `retp`.
 * The value to write is passed via the pointer `valp`.
 *
 * `void _MCF_atomic_xchg_pWIDTH_ORDER(void* retp, void* mem, const void* valp);`  */
#define __MCF_atomic_xchg_p(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  void  \
  _MCF_atomic_xchg_p##SUFFIX(void* __retp, void* __mem, const void* __valp) __MCF_NOEXCEPT  \
    {  \
      INTEGER __val = *(const INTEGER*) __valp;  \
      __val = __atomic_exchange_n((INTEGER*) __mem, __val, ORDER_SUCC);  \
      *(INTEGER*) __retp = __val;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_p, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_p, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_p, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_p, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_xchg_p, cst)

/* Performs a scalar atomic compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_WIDTH_ORDER(void* restrict mem, void* restrict cmpp, INTEGER val);`  */
#define __MCF_atomic_cmpxchg_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_##SUFFIX(void* __restrict __mem, void* __restrict __cmpp, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      INTEGER __cmp = *(INTEGER*) __cmpp;  \
      bool __r = __atomic_compare_exchange_n((INTEGER*) __mem, &__cmp, __val, 0, ORDER_SUCC, ORDER_FAIL);  \
      *(INTEGER*) __cmpp = __cmp;  \
      return __r;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_, cst)

/* Performs a generic atomic compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_pWIDTH_ORDER(void* restrict mem, void* restrict cmpp, const void* val);`  */
#define __MCF_atomic_cmpxchg_p(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_p##SUFFIX(void* __restrict __mem, void* __restrict __cmpp, const void* __valp) __MCF_NOEXCEPT  \
    {  \
      INTEGER __val = *(const INTEGER*) __valp;  \
      INTEGER __cmp = *(INTEGER*) __cmpp;  \
      bool __r = __atomic_compare_exchange_n((INTEGER*) __mem, &__cmp, __val, 0, ORDER_SUCC, ORDER_FAIL);  \
      *(INTEGER*) __cmpp = __cmp;  \
      return __r;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_p, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_p, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_p, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_p, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_atomic_cmpxchg_p, cst)

/* Performs a scalar atomic weak compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_weak_WIDTH_ORDER(void* restrict mem, void* restrict cmpp, INTEGER val);`  */
#define __MCF_ATOMIC_cmpxchg_weak_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_weak_##SUFFIX(void* __restrict __mem, void* __restrict __cmpp, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      INTEGER __cmp = *(INTEGER*) __cmpp;  \
      bool __r = __atomic_compare_exchange_n((INTEGER*) __mem, &__cmp, __val, 1, ORDER_SUCC, ORDER_FAIL);  \
      *(INTEGER*) __cmpp = __cmp;  \
      return __r;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_, cst)

/* Performs a generic atomic weak compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_weak_pWIDTH_ORDER(void* restrict mem, void* restrict cmpp, const void* val);`  */
#define __MCF_ATOMIC_cmpxchg_weak_p(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  bool  \
  _MCF_atomic_cmpxchg_weak_p##SUFFIX(void* __restrict __mem, void* __restrict __cmpp, const void* __valp) __MCF_NOEXCEPT  \
    {  \
      INTEGER __cmp = *(INTEGER*) __cmpp;  \
      INTEGER __val = *(const INTEGER*) __valp;  \
      bool __r = __atomic_compare_exchange_n((INTEGER*) __mem, &__cmp, __val, 1, ORDER_SUCC, ORDER_FAIL);  \
      *(INTEGER*) __cmpp = __cmp;  \
      return __r;  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_p, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_p, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_p, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_p, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_cmpxchg_weak_p, cst)

/* Performs a generic atomic fetch-and-add operation.
 * The value that has been swapped out is returned as an integer.
 * The value to add is passed by value as an integer.
 *
 * `INTEGER val __MCF_atomic_xadd_WIDTH_ORDER(void* mem, INTEGER val);`  */
#define __MCF_ATOMIC_xadd_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xadd_##SUFFIX(void* __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __atomic_fetch_add((INTEGER*) __mem, __val, ORDER_SUCC);  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xadd_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xadd_, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xadd_, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xadd_, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xadd_, cst)

/* Performs a generic atomic fetch-and-subtract operation.
 * The value that has been swapped out is returned as an integer.
 * The value to subtract is passed by value as an integer.
 *
 * `INTEGER val __MCF_atomic_xsub_WIDTH_ORDER(void* mem, INTEGER val);`  */
#define __MCF_ATOMIC_xsub_(INTEGER, SUFFIX, ORDER_SUCC, ORDER_FAIL)  \
  __MCF_ALWAYS_INLINE  \
  INTEGER  \
  _MCF_atomic_xsub_##SUFFIX(void* __mem, INTEGER __val) __MCF_NOEXCEPT  \
    {  \
      return __atomic_fetch_sub((INTEGER*) __mem, __val, ORDER_SUCC);  \
    }

__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xsub_, rlx)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xsub_, acq)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xsub_, rel)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xsub_, arl)
__MCF_ATOMIC_FUNCTIONS(__MCF_ATOMIC_xsub_, cst)

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_ATOMIC_  */
