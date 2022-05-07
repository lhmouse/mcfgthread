/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_ATOMIC_H_
#define __MCFGTHREAD_ATOMIC_H_

#include "fwd.h"

/* We don't use the generic builtins due to a GCC bug.
 * See <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105495> for details.
 *
 * These functions are provided as inlined ones only. Hereinafter, `WIDTH` sets
 * `INTEGER` to `intWIDTH_t`. `ORDER` may be `rlx` for relaxed order, `acq` for
 * acquire order, `rel` for release order, `arl` for both acquire and release
 * order, or `cst` for sequentially-consistent order.  */

/* Performs a scalar atomic load operation.
 * The value that has been read is returned as an integer.
 *
 * `INTEGER _MCF_atomic_load_WIDTH_ORDER(const void* restrict mem);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   INTEGER
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_load_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   const void* __mem

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    return __atomic_load_n((const INTEGER*) __mem, ORDER_SUCC);

#define __MCF_ATOMIC_HAS_ACQUIRE_
#include "atomic_gen.i"

/* Performs a generic atomic load operation.
 * The value that has been read is returned via the pointer `retp`.
 *
 * `void _MCF_atomic_load_pWIDTH_ORDER(void* retp, const void* restrict mem);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   void
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_load_p
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __retp, const void* __mem

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __val = __atomic_load_n((const INTEGER*) __mem, ORDER_SUCC);  \
    *(INTEGER*) __retp = __val;

#define __MCF_ATOMIC_HAS_ACQUIRE_
#include "atomic_gen.i"

/* Performs a scalar atomic store operation.
 * The value to write is passed by value as an integer.
 *
 * `INTEGER _MCF_atomic_store_WIDTH_ORDER(void* restrict mem, INTEGER val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   INTEGER
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_store_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, INTEGER __val

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    __atomic_store_n((INTEGER*) __mem, __val, ORDER_SUCC);  \
    return __val;

#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a generic atomic store operation.
 * The value to write is passed via the pointer `valp`.
 *
 * `void _MCF_atomic_store_pWIDTH_ORDER(void* restrict mem, const void* valp);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   void
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_store_p
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, const void* __valp

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __val = *(const INTEGER*) __valp;  \
    __atomic_store_n((INTEGER*) __mem, __val, ORDER_SUCC);

#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a scalar atomic exchange operation.
 * The previous value is returned as an integer.
 * The value to write is passed by value as an integer.
 *
 * `INTEGER _MCF_atomic_xchg_WIDTH_ORDER(void* restrict mem, INTEGER val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   INTEGER
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_xchg_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, INTEGER __val

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    return __atomic_exchange_n((INTEGER*) __mem, __val, ORDER_SUCC);

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a generic atomic exchange operation.
 * The previous value is returned via the pointer `retp`.
 * The value to write is passed via the pointer `valp`.
 *
 * `void _MCF_atomic_xchg_pWIDTH_ORDER(void* retp, void* restrict mem, const void* valp);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   void
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_xchg_p
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __retp, void* __mem, const void* __valp

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __val = *(const INTEGER*) __valp;  \
    __val = __atomic_exchange_n((INTEGER*) __mem, __val, ORDER_SUCC);  \
    *(INTEGER*) __retp = __val;

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a scalar atomic compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_WIDTH_ORDER(void* restrict mem, void* restrict cmpp, INTEGER val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   bool
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_cmpxchg_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, void* restrict __cmpp, INTEGER __val

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __cmp = *(INTEGER*) __cmpp;  \
    bool __r = __atomic_compare_exchange_n((INTEGER*) __mem,  \
          &__cmp, __val, 0, ORDER_SUCC, ORDER_FAIL);  \
    *(INTEGER*) __cmpp = __cmp;  \
    return __r;

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a generic atomic compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_pWIDTH_ORDER(void* restrict mem, void* restrict cmpp, const void* val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   bool
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_cmpxchg_p
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, void* restrict __cmpp, const void* __valp

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __cmp = *(INTEGER*) __cmpp;  \
    INTEGER __val = *(const INTEGER*) __valp;  \
    bool __r = __atomic_compare_exchange_n((INTEGER*) __mem,  \
          &__cmp, __val, 0, ORDER_SUCC, ORDER_FAIL);  \
    *(INTEGER*) __cmpp = __cmp;  \
    return __r;

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a scalar atomic weak compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_weak_WIDTH_ORDER(void* restrict mem, void* restrict cmpp, INTEGER val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   bool
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_cmpxchg_weak_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, void* restrict __cmpp, INTEGER __val

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __cmp = *(INTEGER*) __cmpp;  \
    bool __r = __atomic_compare_exchange_n((INTEGER*) __mem,  \
          &__cmp, __val, 1, ORDER_SUCC, ORDER_FAIL);  \
    *(INTEGER*) __cmpp = __cmp;  \
    return __r;

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a generic atomic weak compare-and-exchange operation.
 * The value for comparison is passed via the pointer `cmpp`.
 * The value to write is passed by value as an integer.
 *
 * `bool _MCF_atomic_cmpxchg_weak_pWIDTH_ORDER(void* restrict mem, void* restrict cmpp, const void* val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   bool
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_cmpxchg_weak_p
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, void* restrict __cmpp, const void* __valp

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    INTEGER __cmp = *(INTEGER*) __cmpp;  \
    INTEGER __val = *(const INTEGER*) __valp;  \
    bool __r = __atomic_compare_exchange_n((INTEGER*) __mem,  \
          &__cmp, __val, 1, ORDER_SUCC, ORDER_FAIL);  \
    *(INTEGER*) __cmpp = __cmp;  \
    return __r;  \

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a generic atomic fetch-and-add operation.
 * The value that has been swapped out is returned as an integer.
 * The value to add is passed by value as an integer.
 *
 * `INTEGER val __MCF_atomic_xadd_WIDTH_ORDER(void* restrict mem, INTEGER val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   INTEGER
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_xadd_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, INTEGER __val

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    return __atomic_fetch_add((INTEGER*) __mem, __val, ORDER_SUCC);  \

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

/* Performs a generic atomic fetch-and-subtract operation.
 * The value that has been swapped out is returned as an integer.
 * The value to subtract is passed by value as an integer.
 *
 * `INTEGER val __MCF_atomic_xsub_WIDTH_ORDER(void* restrict mem, INTEGER val);`  */
#define __MCF_ATOMIC_RETURN_(INTEGER)   INTEGER
#define __MCF_ATOMIC_FUNCTION_          _MCF_atomic_xsub_
#define __MCF_ATOMIC_PARAMS_(INTEGER)   void* __mem, INTEGER __val

#define __MCF_ATOMIC_FUNC_BODY_(INTEGER, ORDER_SUCC, ORDER_FAIL)  \
    return __atomic_fetch_sub((INTEGER*) __mem, __val, ORDER_SUCC);  \

#define __MCF_ATOMIC_HAS_ACQUIRE_
#define __MCF_ATOMIC_HAS_RELEASE_
#include "atomic_gen.i"

#endif  /* __MCFGTHREAD_ATOMIC_H_  */
