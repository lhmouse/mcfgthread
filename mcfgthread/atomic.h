/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_ATOMIC_
#define __MCFGTHREAD_ATOMIC_
#if !defined __MCF_ATOMIC_GENERATOR_STATE_

#include "fwd.h"

/** Define macros to use atomic operations.
 *
 * If the compiler has builtin support, then it should be preferred; otherwise
 * the standard library is used. Microsoft Visual Studio 2022 has experimental
 * support which seems to suffice.  */
#if defined __GNUC__ || defined __clang__
#  define __MCF_ATOMIC(...)                   volatile __VA_ARGS__
#  define __MCF_memory_order_rlx              __ATOMIC_RELAXED
#  define __MCF_memory_order_acq              __ATOMIC_ACQUIRE
#  define __MCF_memory_order_rel              __ATOMIC_RELEASE
#  define __MCF_memory_order_arl              __ATOMIC_ACQ_REL
#  define __MCF_memory_order_cst              __ATOMIC_SEQ_CST
#  define __MCF_atomic_load(p,o)              __atomic_load_n(p,o)
#  define __MCF_atomic_store(p,v,o)           __atomic_store_n(p,v,o)
#  define __MCF_atomic_xchg(p,v,o)            __atomic_exchange_n(p,v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)     __atomic_compare_exchange_n(p,c,v,0,o,f)
#  define __MCF_atomic_cmpxchg_w(p,c,v,o,f)   __atomic_compare_exchange_n(p,c,v,1,o,f)
#  define __MCF_atomic_xadd(p,v,o)            __atomic_fetch_add(p,v,o)
#  define __MCF_atomic_xsub(p,v,o)            __atomic_fetch_sub(p,v,o)
#  define __MCF_atomic_thread_fence(o)        __atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)        __atomic_signal_fence(o)
#else
#  if __MCF_CXX11(1+)0
#    include <atomic>
#    define __MCF_ATOMIC(...)                 ::std::atomic<__VA_ARGS__>
#  else
#    include <stdatomic.h>
#    define __MCF_ATOMIC(...)                 _Atomic __VA_ARGS__
#  endif
#  define __MCF_memory_order_rlx              __MCF_CXX11(::std::)memory_order_relaxed
#  define __MCF_memory_order_acq              __MCF_CXX11(::std::)memory_order_acquire
#  define __MCF_memory_order_rel              __MCF_CXX11(::std::)memory_order_release
#  define __MCF_memory_order_arl              __MCF_CXX11(::std::)memory_order_acq_rel
#  define __MCF_memory_order_cst              __MCF_CXX11(::std::)memory_order_seq_cst
#  define __MCF_atomic_load(p,o)              __MCF_CXX11(::std::)atomic_load_explicit(p,o)
#  define __MCF_atomic_store(p,v,o)           __MCF_CXX11(::std::)atomic_store_explicit(p,v,o)
#  define __MCF_atomic_xchg(p,v,o)            __MCF_CXX11(::std::)atomic_exchange_explicit(p,v,o)
#  define __MCF_atomic_cmpxchg(p,c,v,o,f)     __MCF_CXX11(::std::)atomic_compare_exchange_strong_explicit(p,c,v,o,f)
#  define __MCF_atomic_cmpxchg_w(p,c,v,o,f)   __MCF_CXX11(::std::)atomic_compare_exchange_weak_explicit(p,c,v,o,f)
#  define __MCF_atomic_xadd(p,v,o)            __MCF_CXX11(::std::)atomic_fetch_add_explicit(p,v,o)
#  define __MCF_atomic_xsub(p,v,o)            __MCF_CXX11(::std::)atomic_fetch_sub_explicit(p,v,o)
#  define __MCF_atomic_thread_fence(o)        __MCF_CXX11(::std::)atomic_thread_fence(o)
#  define __MCF_atomic_signal_fence(o)        __MCF_CXX11(::std::)atomic_signal_fence(o)
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_IMPORT
#  define __MCF_ATOMIC_INLINE  __MCF_ALWAYS_INLINE
#endif

#pragma push_macro("WIDTH")
#pragma push_macro("INTEGER")
#pragma push_macro("ORDER")
#pragma push_macro("ORDER_A")
#pragma push_macro("ORDER_R")
#pragma push_macro("E_ATOMICIFY")
#pragma push_macro("E_ORDER")
#pragma push_macro("E_ORDER_A")
#pragma push_macro("E_ORDER_R")

#undef WIDTH
#undef INTEGER
#undef ORDER
#undef ORDER_A
#undef ORDER_R
#undef E_ATOMICIFY
#undef E_ORDER
#undef E_ORDER_A
#undef E_ORDER_R

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

#  define E_ATOMICIFY(...)  __MCF_CAST_PTR(__MCF_ATOMIC(INTEGER), __VA_ARGS__)
#  define E_ORDER           __MCF_C2(__MCF_memory_order,ORDER)
#  define E_ORDER_A         __MCF_C2(__MCF_memory_order,ORDER_A)
#  define E_ORDER_R         __MCF_C2(__MCF_memory_order,ORDER_R)

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

#  undef E_ATOMICIFY
#  undef E_ORDER
#  undef E_ORDER_A
#  undef E_ORDER_R

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 20001

/** Performs an atomic load operation.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @returns the value of the atomic object as an integer.  */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_load_,WIDTH,ORDER) (const volatile void* __mem)
  __MCF_noexcept
  {
    return __MCF_atomic_load(E_ATOMICIFY(__mem), E_ORDER_A);
  }

/** Performs an atomic load operation.
 *
 * @param `res` shall point to a variable of the specified width, which receives
 *     the value of the atomic object.
 * @param `mem` shall point to an atomic object of the specified width.
 * @returns nothing.  */
__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_load_p,WIDTH,ORDER) (void* __res, const volatile void* __mem)
  __MCF_noexcept
  {
    INTEGER __rval = __MCF_atomic_load(E_ATOMICIFY(__mem), E_ORDER_A);
    *(INTEGER*) __res = __rval;
  }

/** Performs an atomic store operation.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `val` is the value to store into the atomic object.
 * @returns nothing.  */
__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_store_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val)
  __MCF_noexcept
  {
    __MCF_atomic_store(E_ATOMICIFY(__mem), __val, E_ORDER_R);
  }

/** Performs an atomic store operation.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `src` shall point to a variable of the specified width, which provides
 *     the value to store into the atomic object.
 * @returns nothing.  */
__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_store_p,WIDTH,ORDER) (volatile void* __mem, const void* __src)
  __MCF_noexcept
  {
    INTEGER __val = *(const INTEGER*) __src;
    __MCF_atomic_store(E_ATOMICIFY(__mem), __val, E_ORDER_R);
  }

/** Performs an atomic exchange operation.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `val` is the value to store into the atomic object.
 * @returns the previous value of the atomic object as an integer.  */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_xchg_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val)
  __MCF_noexcept
  {
    return __MCF_atomic_xchg(E_ATOMICIFY(__mem), __val, E_ORDER);
  }

/** Performs an atomic exchange operation.
 *
 * @param `res` shall point to a variable of the specified width, which receives
 *     the previous value of the atomic object.
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `src` shall point to a variable of the specified width, which provides
 *     the value to store into the atomic object.
 * @returns nothing.  */
__MCF_ATOMIC_INLINE
void
__MCF_C3(_MCF_atomic_xchg_p,WIDTH,ORDER) (void* __res, volatile void* __mem, const void* __src)
  __MCF_noexcept
  {
    INTEGER __val = *(const INTEGER*) __src;
    INTEGER __rval = __MCF_atomic_xchg(E_ATOMICIFY(__mem), __val, E_ORDER);
    *(INTEGER*) __res = __rval;
  }

/** Performs an atomic compare-and-exchange operation.
 *
 * If the value of `*mem` compares equal to `*cmp`, then `val` is stored into
 * `*mem` and the function succeeds; otherwise, the value of `*mem` is loaded
 * into `*cmp` and the function fails. The operation is performed atomically.
 * On some architectures, this function may create a loop to recover from
 * spurious failures.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `cmp` shall point to a variable of the specified width, which provides
 *     the value to compare, and when the function fails, receives the current
 *     value of the atomic object.
 * @param `val` is the value to store into the atomic object upon success.
 * @returns whether the comparison succeeds.  */
__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_,WIDTH,ORDER) (volatile void* __mem, INTEGER* __cmp, INTEGER __val)
  __MCF_noexcept
  {
    return __MCF_atomic_cmpxchg(E_ATOMICIFY(__mem), __cmp, __val, E_ORDER, E_ORDER_A);
  }

/** Performs an atomic compare-and-exchange operation.
 *
 * If the value of `*mem` compares equal to `*cmp`, then `*src` is stored into
 * `*mem` and the function succeeds; otherwise, the value of `*mem` is loaded
 * into `*cmp` and the function fails. The operation is performed atomically.
 * On some architectures, this function may create a loop to recover from
 * spurious failures.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `cmp` shall point to a variable of the specified width, which provides
 *     the value to compare, and when the function fails, receives the current
 *     value of the atomic object.
 * @param `src` shall point to a variable of the specified width, which provides
 *     the value to store into the atomic object.
 * @returns whether the comparison succeeds.  */
__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_p,WIDTH,ORDER) (volatile void* __mem, void* __cmp, const void* __src)
  __MCF_noexcept
  {
    INTEGER __cval = *(const INTEGER*) __cmp;
    INTEGER __val = *(const INTEGER*) __src;
    bool __succ = __MCF_atomic_cmpxchg(E_ATOMICIFY(__mem), &__cval, __val, E_ORDER, E_ORDER_A);
    *(INTEGER*) __cmp = __cval;
    return __succ;
  }

/** Performs a weak atomic compare-and-exchange operation.
 *
 * If the value of `*mem` compares equal to `*cmp`, then `val` is stored into
 * `*mem` and the function succeeds; otherwise, the value of `*mem` is loaded
 * into `*cmp` and the function fails. The operation is performed atomically.
 * On some architectures, this function may fail spuriously.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `cmp` shall point to a variable of the specified width, which provides
 *     the value to compare, and when the function fails, receives the current
 *     value of the atomic object.
 * @param `val` is the value to store into the atomic object upon success.
 * @returns whether the comparison succeeds.  */
__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_weak_,WIDTH,ORDER) (volatile void* __mem, INTEGER* __cmp, INTEGER __val)
  __MCF_noexcept
  {
    return __MCF_atomic_cmpxchg_w(E_ATOMICIFY(__mem), __cmp, __val, E_ORDER, E_ORDER_A);
  }

/** Performs a weak atomic compare-and-exchange operation.
 *
 * If the value of `*mem` compares equal to `*cmp`, then `*src` is stored into
 * `*mem` and the function succeeds; otherwise, the value of `*mem` is loaded
 * into `*cmp` and the function fails. The operation is performed atomically.
 * On some architectures, this function may fail spuriously.
 *
 * @param `mem` shall point to an atomic object of the specified width.
 * @param `cmp` shall point to a variable of the specified width, which provides
 *     the value to compare, and when the function fails, receives the current
 *     value of the atomic object.
 * @param `src` shall point to a variable of the specified width, which provides
 *     the value to store into the atomic object.
 * @returns whether the comparison succeeds.  */
__MCF_ATOMIC_INLINE
bool
__MCF_C3(_MCF_atomic_cmpxchg_weak_p,WIDTH,ORDER) (volatile void* __mem, void* __cmp, const void* __src)
  __MCF_noexcept
  {
    INTEGER __cval = *(const INTEGER*) __cmp;
    INTEGER __val = *(const INTEGER*) __src;
    bool __succ = __MCF_atomic_cmpxchg_w(E_ATOMICIFY(__mem), &__cval, __val, E_ORDER, E_ORDER_A);
    *(INTEGER*) __cmp = __cval;
    return __succ;
  }

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 20002

/** Adds `val` to the value of an atomic integer.
 *
 * @param `mem` shall point to an atomic integer of the specified width.
 * @param `val` is the value to add to the atomic integer.
 * @returns the previous value of the atomic integer.  */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_xadd_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val)
  __MCF_noexcept
  {
    return __MCF_atomic_xadd(E_ATOMICIFY(__mem), __val, E_ORDER);
  }

/** Subtracts `val` from the value of an atomic integer.
 *
 * @param `mem` shall point to an atomic integer of the specified width.
 * @param `val` is the value to subtract from the atomic integer.
 * @returns the previous value of the atomic integer.  */
__MCF_ATOMIC_INLINE
INTEGER
__MCF_C3(_MCF_atomic_xsub_,WIDTH,ORDER) (volatile void* __mem, INTEGER __val)
  __MCF_noexcept
  {
    return __MCF_atomic_xsub(E_ATOMICIFY(__mem), __val, E_ORDER);
  }

#elif __MCF_ATOMIC_GENERATOR_STATE_ == 20003

/** Emits hardware memory fence instructions to ensure that the effects of memory
 * operations are visible in the requested memory order.
 *
 * @returns nothing.  */
__MCF_ATOMIC_INLINE
void
__MCF_C2(_MCF_thread_fence,ORDER) (void)
  __MCF_noexcept
  {
    __MCF_atomic_thread_fence(E_ORDER);
  }

/** Prevents the compiler from reordering other instructions which would violate
 * the requested memory order, without generating any hardware instructions.
 *
 * @returns nothing.  */
__MCF_ATOMIC_INLINE
void
__MCF_C2(_MCF_signal_fence,ORDER) (void)
  __MCF_noexcept
  {
    __MCF_atomic_signal_fence(E_ORDER);
  }

#else  /* `__MCF_ATOMIC_GENERATOR_STATE_` final */

#pragma pop_macro("WIDTH")
#pragma pop_macro("INTEGER")
#pragma pop_macro("ORDER")
#pragma pop_macro("ORDER_A")
#pragma pop_macro("ORDER_R")
#pragma pop_macro("E_ATOMICIFY")
#pragma pop_macro("E_ORDER")
#pragma pop_macro("E_ORDER_A")
#pragma pop_macro("E_ORDER_R")

__MCF_CXX(})  /* extern "C"  */
#endif  /* `__MCF_ATOMIC_GENERATOR_STATE_` defined  */
#endif  /* __MCFGTHREAD_ATOMIC_  */
