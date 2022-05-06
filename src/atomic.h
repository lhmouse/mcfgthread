/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_ATOMIC_H_
#define __MCFGTHREAD_ATOMIC_H_

#include "fwd.h"

/* Generic atomic load
 * Note the order of arguments is swapped. Output precedes input.  */
#define __MCF_ATOMIC_LOAD_PTR_RLX(out, mem)   __atomic_load(mem, out, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_LOAD_PTR_ACQ(out, mem)   __atomic_load(mem, out, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_LOAD_PTR_CST(out, mem)   __atomic_load(mem, out, __ATOMIC_SEQ_CST)

/* Generic atomic store  */
#define __MCF_ATOMIC_STORE_PTR_RLX(mem, in)   __atomic_store(mem, in, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_STORE_PTR_REL(mem, in)   __atomic_store(mem, in, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_STORE_PTR_CST(mem, in)   __atomic_store(mem, in, __ATOMIC_SEQ_CST)

/* Generic atomic exchange
 * Note the order of arguments is swapped. Output precedes input.  */
#define __MCF_ATOMIC_XCHG_PTR_RLX(out, mem, in)   __atomic_exchange(mem, in, out, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XCHG_PTR_ACQ(out, mem, in)   __atomic_exchange(mem, in, out, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XCHG_PTR_REL(out, mem, in)   __atomic_exchange(mem, in, out, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XCHG_PTR_ARL(out, mem, in)   __atomic_exchange(mem, in, out, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XCHG_PTR_CST(out, mem, in)   __atomic_exchange(mem, in, out, __ATOMIC_SEQ_CST)

/* Generic strong compare-and-exchange  */
#define __MCF_ATOMIC_CMPXCHG_PTR_RLX(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_PTR_ACQ(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_PTR_REL(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_PTR_ARL(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_PTR_CST(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

/* Generic weak compare-and-exchange  */
#define __MCF_ATOMIC_CMPXCHG_WEAK_PTR_RLX(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_PTR_ACQ(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, true, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_PTR_REL(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_PTR_ARL(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_PTR_CST(mem, cmp, in)   __atomic_compare_exchange(mem, cmp, in, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

/* Scalar atomic load  */
#define __MCF_ATOMIC_LOAD_RLX(mem)   __atomic_load_n(mem, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_LOAD_ACQ(mem)   __atomic_load_n(mem, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_LOAD_CST(mem)   __atomic_load_n(mem, __ATOMIC_SEQ_CST)

/* Scalar atomic store  */
#define __MCF_ATOMIC_STORE_RLX(mem, val)   __atomic_store_n(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_STORE_REL(mem, val)   __atomic_store_n(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_STORE_CST(mem, val)   __atomic_store_n(mem, val, __ATOMIC_SEQ_CST)

/* Scalar atomic exchange  */
#define __MCF_ATOMIC_XCHG_RLX(mem, val)   __atomic_exchange_n(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XCHG_ACQ(mem, val)   __atomic_exchange_n(mem, val, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XCHG_REL(mem, val)   __atomic_exchange_n(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XCHG_ARL(mem, val)   __atomic_exchange_n(mem, val, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XCHG_CST(mem, val)   __atomic_exchange_n(mem, val, __ATOMIC_SEQ_CST)

/* Scalar strong compare-and-exchange  */
#define __MCF_ATOMIC_CMPXCHG_RLX(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_ACQ(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_REL(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_ARL(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_CST(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

/* Scalar weak compare-and-exchange  */
#define __MCF_ATOMIC_CMPXCHG_WEAK_RLX(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_ACQ(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, true, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_REL(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_ARL(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_CST(mem, cmp, val)   __atomic_compare_exchange_n(mem, cmp, val, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

/* Scalar exchange-and-add  */
#define __MCF_ATOMIC_ADD_RLX(mem, val)   __atomic_fetch_add(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_ADD_ACQ(mem, val)   __atomic_fetch_add(mem, val, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_ADD_REL(mem, val)   __atomic_fetch_add(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_ADD_ARL(mem, val)   __atomic_fetch_add(mem, val, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_ADD_CST(mem, val)   __atomic_fetch_add(mem, val, __ATOMIC_SEQ_CST)

/* Scalar exchange-and-sub  */
#define __MCF_ATOMIC_SUB_RLX(mem, val)   __atomic_fetch_sub(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_SUB_ACQ(mem, val)   __atomic_fetch_sub(mem, val, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_SUB_REL(mem, val)   __atomic_fetch_sub(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_SUB_ARL(mem, val)   __atomic_fetch_sub(mem, val, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_SUB_CST(mem, val)   __atomic_fetch_sub(mem, val, __ATOMIC_SEQ_CST)

/* Scalar exchange-and-bitwise-and  */
#define __MCF_ATOMIC_AND_RLX(mem, val)   __atomic_fetch_and(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_AND_ACQ(mem, val)   __atomic_fetch_and(mem, val, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_AND_REL(mem, val)   __atomic_fetch_and(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_AND_ARL(mem, val)   __atomic_fetch_and(mem, val, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_AND_CST(mem, val)   __atomic_fetch_and(mem, val, __ATOMIC_SEQ_CST)

/* Scalar exchange-and-bitwise-or  */
#define __MCF_ATOMIC_OR_RLX(mem, val)   __atomic_fetch_or(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_OR_ACQ(mem, val)   __atomic_fetch_or(mem, val, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_OR_REL(mem, val)   __atomic_fetch_or(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_OR_ARL(mem, val)   __atomic_fetch_or(mem, val, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_OR_CST(mem, val)   __atomic_fetch_or(mem, val, __ATOMIC_SEQ_CST)

/* Scalar exchange-and-bitwise-xor  */
#define __MCF_ATOMIC_XOR_RLX(mem, val)   __atomic_fetch_xor(mem, val, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XOR_ACQ(mem, val)   __atomic_fetch_xor(mem, val, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XOR_REL(mem, val)   __atomic_fetch_xor(mem, val, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XOR_ARL(mem, val)   __atomic_fetch_xor(mem, val, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XOR_CST(mem, val)   __atomic_fetch_xor(mem, val, __ATOMIC_SEQ_CST)

#endif  /* __MCFGTHREAD_ATOMIC_H_  */
