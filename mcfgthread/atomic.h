/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_ATOMIC_
#define __MCFGTHREAD_ATOMIC_
#if !defined __MCF_ATOMIC_GENERATOR_STATE_

#include "fwd.h"

#if defined __GNUC__ || defined __clang__

/* Use built-in types.  */
#  define __MCF_ATOMICIFY(T, ...)    __MCF_CAST_PTR(volatile T, __VA_ARGS__)

#  define __MCF_memory_order_rlx     __ATOMIC_RELAXED
#  define __MCF_memory_order_acq     __ATOMIC_ACQUIRE
#  define __MCF_memory_order_rel     __ATOMIC_RELEASE
#  define __MCF_memory_order_arl     __ATOMIC_ACQ_REL
#  define __MCF_memory_order_cst     __ATOMIC_SEQ_CST

#  define __MCF_atomic_load(p,o)              __atomic_load_n(p,o)
#  define __MCF_atomic_store(p,v,o)           __atomic_store_n(p,v,o)
#  define __MCF_atomic_xchg(p,v,o)            __atomic_exchange_n(p,v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)     __atomic_compare_exchange_n(p,c,v,0,o,f)
#  define __MCF_atomic_cmpxchg_w(p,c,v,o,f)   __atomic_compare_exchange_n(p,c,v,1,o,f)
#  define __MCF_atomic_xadd(p,v,o)            __atomic_fetch_add(p,v,o)
#  define __MCF_atomic_xsub(p,v,o)            __atomic_fetch_sub(p,v,o)
#  define __MCF_atomic_thread_fence(o)        __atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)        __atomic_signal_fence(o)

#elif __MCF_CXX11(1+)0

/* Use the C++11 standard library.  */
#  include <atomic>
#  define __MCF_ATOMICIFY(T, ...)    __MCF_CAST_PTR(::std::atomic<T>, __VA_ARGS__)

#  define __MCF_memory_order_rlx     ::std::memory_order_relaxed
#  define __MCF_memory_order_acq     ::std::memory_order_acquire
#  define __MCF_memory_order_rel     ::std::memory_order_release
#  define __MCF_memory_order_arl     ::std::memory_order_acq_rel
#  define __MCF_memory_order_cst     ::std::memory_order_seq_cst

#  define __MCF_atomic_load(p,o)              (p)->load(o)
#  define __MCF_atomic_store(p,v,o)           (p)->store(v,o)
#  define __MCF_atomic_xchg(p,v,o)            (p)->exchange(v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)     (p)->compare_exchange_strong(*(c),v,o,f)
#  define __MCF_atomic_cmpxchg_w(p,c,v,o,f)   (p)->compare_exchange_weak(*(c),v,o,f)
#  define __MCF_atomic_xadd(p,v,o)            (p)->fetch_add(v,o)
#  define __MCF_atomic_xsub(p,v,o)            (p)->fetch_sub(v,o)
#  define __MCF_atomic_thread_fence(o)        ::std::atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)        ::std::atomic_signal_fence(o)

#else

/* Use the C11 standard library. Microsoft Visual Studio 2022 has experimental
 * support which seems to suffice.  */
#  include <stdatomic.h>
#  define __MCF_ATOMICIFY(T, ...)    __MCF_CAST_PTR(_Atomic T, __VA_ARGS__)

#  define __MCF_memory_order_rlx     memory_order_relaxed
#  define __MCF_memory_order_acq     memory_order_acquire
#  define __MCF_memory_order_rel     memory_order_release
#  define __MCF_memory_order_arl     memory_order_acq_rel
#  define __MCF_memory_order_cst     memory_order_seq_cst

#  define __MCF_atomic_load(p,o)              atomic_load_explicit(p,o)
#  define __MCF_atomic_store(p,v,o)           atomic_store_explicit(p,v,o)
#  define __MCF_atomic_xchg(p,v,o)            atomic_exchange_explicit(p,v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)     atomic_compare_exchange_strong_explicit(p,c,v,o,f)
#  define __MCF_atomic_cmpxchg_w(p,c,v,o,f)   atomic_compare_exchange_weak_explicit(p,c,v,o,f)
#  define __MCF_atomic_xadd(p,v,o)            atomic_fetch_add_explicit(p,v,o)
#  define __MCF_atomic_xsub(p,v,o)            atomic_fetch_sub_explicit(p,v,o)
#  define __MCF_atomic_thread_fence(o)        atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)        atomic_signal_fence(o)

#endif  /* __MCF_atomic  */

__MCF_CXX(extern "C" {)
#define __MCF_ATOMIC_INLINE  __MCF_ALWAYS_INLINE

#pragma push_macro("WIDTH")
#pragma push_macro("INTEGER")
#pragma push_macro("ORDER")
#pragma push_macro("ORDER_A")
#pragma push_macro("ORDER_R")

#undef WIDTH
#undef INTEGER
#undef ORDER
#undef ORDER_A
#undef ORDER_R

#define __MCF_ATOMIC_GENERATOR_STATE_   10001
#undef __MCFGTHREAD_ATOMIC_
#include __FILE__
#undef __MCF_ATOMIC_GENERATOR_STATE_

#define __MCF_ATOMIC_GENERATOR_STATE_   65535
#undef __MCFGTHREAD_ATOMIC_
#include __FILE__
#undef __MCF_ATOMIC_GENERATOR_STATE_

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 10001

#  define ORDER   _rlx
#  define ORDER_A  _rlx
#  define ORDER_R  _rlx
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   10002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef ORDER
#  undef ORDER_A
#  undef ORDER_R

#  define ORDER   _acq
#  define ORDER_A  _acq
#  define ORDER_R  _rlx
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   10002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef ORDER
#  undef ORDER_A
#  undef ORDER_R

#  define ORDER   _rel
#  define ORDER_A  _rlx
#  define ORDER_R  _rel
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   10002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef ORDER
#  undef ORDER_A
#  undef ORDER_R

#  define ORDER   _arl
#  define ORDER_A  _acq
#  define ORDER_R  _rel
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   10002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef ORDER
#  undef ORDER_A
#  undef ORDER_R

#  define ORDER   _cst
#  define ORDER_A  _cst
#  define ORDER_R  _cst
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   10002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef ORDER
#  undef ORDER_A
#  undef ORDER_R

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 10002

#  define WIDTH   b
#  define INTEGER  bool
#  undef __MCFGTHREAD_ATOMIC_
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  define WIDTH   8
#  define INTEGER  int8_t
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  define WIDTH   16
#  define INTEGER  int16_t
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  define WIDTH   32
#  define INTEGER  int32_t
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  define WIDTH   64
#  define INTEGER  int64_t
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  define WIDTH   ptr
#  define INTEGER  intptr_t
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  define WIDTH   z
#  define INTEGER  size_t
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20001
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20002
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__
#  undef WIDTH
#  undef INTEGER

#  undef __MCF_ATOMIC_GENERATOR_STATE_
#  define __MCF_ATOMIC_GENERATOR_STATE_   20003
#  undef __MCFGTHREAD_ATOMIC_
#  include __FILE__

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 20001

/* Perform an atomic load operation. `mem` shall point to an atomic object of the
 * specified width. The first function returns the value as an integer. The second
 * function writes the value into `*res`.
 *
 *   [INTEGER]
 *   _MCF_atomic_load_[WIDTH]_[ORDER] (const volatile void* mem);
 *
 *   void
 *   _MCF_atomic_load_p[WIDTH]_[ORDER] (void* res, const volatile void* mem);
 */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_load_,WIDTH,ORDER) (const volatile void* __mem) __MCF_noexcept
  {
    return __MCF_atomic_load(__MCF_ATOMICIFY(INTEGER, __mem),
                             __MCF_C2(__MCF_memory_order,ORDER_A));
  }

__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_load_p,WIDTH,ORDER) (void* __res, const volatile void* __mem) __MCF_noexcept
  {
    INTEGER __rval = __MCF_atomic_load(__MCF_ATOMICIFY(INTEGER, __mem),
                                       __MCF_C2(__MCF_memory_order,ORDER_A));
    *(INTEGER*) __res = __rval;
  }

/* Perform an atomic store operation. `mem` shall point to an atomic object of the
 * specified width. The first function stores the integer `val`. The second function
 * stores the value at `*src`.
 *
 *   void
 *   _MCF_atomic_store_[WIDTH]_[ORDER] (volatile void* mem, [INTEGER] val);
 *
 *   void
 *   _MCF_atomic_store_p[WIDTH]_[ORDER] (volatile void* mem, const void* src);
 */
__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_store_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val) __MCF_noexcept
  {
    __MCF_atomic_store(__MCF_ATOMICIFY(INTEGER, __mem), __val,
                       __MCF_C2(__MCF_memory_order,ORDER_R));
  }

__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_store_p,WIDTH,ORDER) (volatile void* __mem, const void* __src) __MCF_noexcept
  {
    INTEGER __val = *(const INTEGER*) __src;
    __MCF_atomic_store(__MCF_ATOMICIFY(INTEGER, __mem), __val,
                       __MCF_C2(__MCF_memory_order,ORDER_R));
  }

/* Perform an atomic exchange operation. `mem` shall point to an atomic object of
 * the specified width. The first function stores the integer `val` and returns the
 * old value as an integer. The second function stores the value at `*src` and
 * writes the old value into `*res`.
 *
 *   [INTEGER]
 *   _MCF_atomic_xchg_[WIDTH]_[ORDER] (volatile void* mem, [INTEGER] val);
 *
 *   void
 *   _MCF_atomic_xchg_p[WIDTH]_[ORDER] (void* res, volatile void* mem, const void* src);
 */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_xchg_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val) __MCF_noexcept
  {
    return __MCF_atomic_xchg(__MCF_ATOMICIFY(INTEGER, __mem), __val,
                             __MCF_C2(__MCF_memory_order,ORDER));
  }

__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_xchg_p,WIDTH,ORDER) (void* __res, volatile void* __mem, const void* __src) __MCF_noexcept
  {
    INTEGER __val = *(const INTEGER*) __src;
    INTEGER __rval = __MCF_atomic_xchg(__MCF_ATOMICIFY(INTEGER, __mem), __val,
                                       __MCF_C2(__MCF_memory_order,ORDER));
    *(INTEGER*) __res = __rval;
  }

/* Perform a strong atomic compare-and-exchange operation. `mem` shall point to an
 * atomic object of the specified width. These functions compare the value of the
 * atomic object with `*cmp`. If they equal, these functions store the integer `val`
 * or the value at `*src` into `*mem`, and return `true`; otherwise these function
 * returns `false`. The old value of `*mem` is stored into `*cmp`. For some targets,
 * these functions may perform the operation as a loop and will not fail spuriously.
 *
 *   bool
 *   _MCF_atomic_cmpxchg_[WIDTH]_[ORDER] (volatile void* mem, [INTEGER]* cmp, [INTEGER] val);
 *
 *   bool
 *   _MCF_atomic_cmpxchg_p[WIDTH]_[ORDER] (volatile void* mem, void* cmp, const void* src);
 */
__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_,WIDTH,ORDER) (volatile void* __mem, INTEGER* __cmp, INTEGER __val) __MCF_noexcept
  {
    return __MCF_atomic_cmpxchg(__MCF_ATOMICIFY(INTEGER, __mem), __cmp, __val,
                                __MCF_C2(__MCF_memory_order,ORDER),
                                __MCF_C2(__MCF_memory_order,ORDER_A));
  }

__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_p,WIDTH,ORDER) (volatile void* __mem, void* __cmp, const void* __src) __MCF_noexcept
  {
    INTEGER __cval = *(const INTEGER*) __cmp;
    INTEGER __val = *(const INTEGER*) __src;
    bool __succ = __MCF_atomic_cmpxchg(__MCF_ATOMICIFY(INTEGER, __mem), &__cval, __val,
                                       __MCF_C2(__MCF_memory_order,ORDER),
                                       __MCF_C2(__MCF_memory_order,ORDER_A));
    *(INTEGER*) __cmp = __cval;
    return __succ;
  }

/* Perform a weak atomic compare-and-exchange operation. `mem` shall point to an
 * atomic object of the specified width. These functions compare the value of the
 * atomic object with `*cmp`. If they equal, these functions store the integer `val`
 * or the value at `*src` into `*mem`, and return `true`; otherwise these function
 * returns `false`. The old value of `*mem` is stored into `*cmp`. For some targets,
 * these functions may fail spuriously.
 *
 *   bool
 *   _MCF_atomic_cmpxchg_weak_[WIDTH]_[ORDER] (volatile void* mem, [INTEGER]* cmp, [INTEGER] val);
 *
 *   bool
 *   _MCF_atomic_cmpxchg_weak_p[WIDTH]_[ORDER] (volatile void* mem, void* cmp, const void* src);
 */
__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_weak_,WIDTH,ORDER) (volatile void* __mem, INTEGER* __cmp, INTEGER __val) __MCF_noexcept
  {
    return __MCF_atomic_cmpxchg_w(__MCF_ATOMICIFY(INTEGER, __mem), __cmp, __val,
                                  __MCF_C2(__MCF_memory_order,ORDER),
                                  __MCF_C2(__MCF_memory_order,ORDER_A));
  }

__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_weak_p,WIDTH,ORDER) (volatile void* __mem, void* __cmp, const void* __src) __MCF_noexcept
  {
    INTEGER __cval = *(const INTEGER*) __cmp;
    INTEGER __val = *(const INTEGER*) __src;
    bool __succ = __MCF_atomic_cmpxchg_w(__MCF_ATOMICIFY(INTEGER, __mem), &__cval, __val,
                                         __MCF_C2(__MCF_memory_order,ORDER),
                                         __MCF_C2(__MCF_memory_order,ORDER_A));
    *(INTEGER*) __cmp = __cval;
    return __succ;
  }

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 20002

/* Add `val` to, or subtract `val` from, an atomic integer. `mem` shall point to an
 * atomic integer of the specified width. These functions return the old value.
 *
 *   [INTEGER]
 *   _MCF_atomic_xadd_[WIDTH]_[ORDER] (volatile void* mem, [INTEGER] val);
 *
 *   [INTEGER]
 *   _MCF_atomic_xsub_[WIDTH]_[ORDER] (volatile void* mem, [INTEGER] val);
 */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_xadd_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val) __MCF_noexcept
  {
    return __MCF_atomic_xadd(__MCF_ATOMICIFY(INTEGER, __mem), __val,
                             __MCF_C2(__MCF_memory_order,ORDER));
  }

__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_xsub_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val) __MCF_noexcept
  {
    return __MCF_atomic_xsub(__MCF_ATOMICIFY(INTEGER, __mem), __val,
                             __MCF_C2(__MCF_memory_order,ORDER));
  }

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 20003

/* Emit a fence (barrier). This ensures that read and write operations shall happen
 * in the requested memory order. A signal fence generates no CPU instruction, but
 * prevents the compiler from reordering load and store operations.
 *
 *   void
 *   _MCF_thread_fence_[ORDER] (void);
 *
 *   void
 *   _MCF_signal_fence_[ORDER] (void);
 */
__MCF_ATOMIC_INLINE
void
__MCF_C2(_MCF_thread_fence,ORDER) (void) __MCF_noexcept
  {
    __MCF_atomic_thread_fence(__MCF_C2(__MCF_memory_order,ORDER));
  }

__MCF_ATOMIC_INLINE
void
__MCF_C2(_MCF_signal_fence,ORDER) (void) __MCF_noexcept
  {
    __MCF_atomic_signal_fence(__MCF_C2(__MCF_memory_order,ORDER));
  }

#else  /* `__MCF_ATOMIC_GENERATOR_STATE_` final */

#pragma pop_macro("WIDTH")
#pragma pop_macro("INTEGER")
#pragma pop_macro("ORDER")
#pragma pop_macro("ORDER_A")
#pragma pop_macro("ORDER_R")

__MCF_CXX(})  /* extern "C"  */
#endif  /* `__MCF_ATOMIC_GENERATOR_STATE_` defined  */
#endif  /* __MCFGTHREAD_ATOMIC_  */
