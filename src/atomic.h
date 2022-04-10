// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_ATOMIC_H_
#define __MCFGTHREAD_ATOMIC_H_

#include "fwd.h"

// Generic atomic load
// Note the order of arguments is swapped. Output precedes input.
#define __MCF_ATOMIC_LOAD_RLX(p,m)   __atomic_load(m, p, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_LOAD_ACQ(p,m)   __atomic_load(m, p, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_LOAD_CST(p,m)   __atomic_load(m, p, __ATOMIC_SEQ_CST)

// Scalar atomic load
#define __MCF_ATOMIC_LOAD_N_RLX(m)   __atomic_load_n(m, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_LOAD_N_ACQ(m)   __atomic_load_n(m, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_LOAD_N_CST(m)   __atomic_load_n(m, __ATOMIC_SEQ_CST)

// Generic atomic store
#define __MCF_ATOMIC_STORE_RLX(m,p)   __atomic_store(m, p, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_STORE_REL(m,p)   __atomic_store(m, p, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_STORE_CST(m,p)   __atomic_store(m, p, __ATOMIC_SEQ_CST)

// Scalar atomic store
#define __MCF_ATOMIC_STORE_N_RLX(m,r)   __atomic_store_n(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_STORE_N_REL(m,r)   __atomic_store_n(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_STORE_N_CST(m,r)   __atomic_store_n(m, r, __ATOMIC_SEQ_CST)

// Generic atomic exchange
// Note the order of arguments is swapped. Output precedes input.
#define __MCF_ATOMIC_XCHG_RLX(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XCHG_ACQ(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XCHG_REL(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XCHG_ARL(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XCHG_CST(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_SEQ_CST)

// Scalar atomic exchange
#define __MCF_ATOMIC_XCHG_N_RLX(m,r)   __atomic_exchange_n(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XCHG_N_ACQ(m,r)   __atomic_exchange_n(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XCHG_N_REL(m,r)   __atomic_exchange_n(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XCHG_N_ARL(m,r)   __atomic_exchange_n(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XCHG_N_CST(m,r)   __atomic_exchange_n(m, r, __ATOMIC_SEQ_CST)

// Generic strong compare-and-exchange
#define __MCF_ATOMIC_CMPXCHG_RLX(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_ACQ(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_REL(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_ARL(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_CST(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Scalar strong compare-and-exchange
#define __MCF_ATOMIC_CMPXCHG_N_RLX(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_N_ACQ(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_N_REL(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_N_ARL(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_N_CST(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Generic weak compare-and-exchange
#define __MCF_ATOMIC_CMPXCHG_WEAK_RLX(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_ACQ(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_REL(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_ARL(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_CST(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Scalar weak compare-and-exchange
#define __MCF_ATOMIC_CMPXCHG_WEAK_N_RLX(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_N_ACQ(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_N_REL(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CMPXCHG_WEAK_N_ARL(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CMPXCHG_WEAK_N_CST(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Scalar exchange-and-add
#define __MCF_ATOMIC_ADD_RLX(m,r)   __atomic_fetch_add(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_ADD_ACQ(m,r)   __atomic_fetch_add(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_ADD_REL(m,r)   __atomic_fetch_add(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_ADD_ARL(m,r)   __atomic_fetch_add(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_ADD_CST(m,r)   __atomic_fetch_add(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-sub
#define __MCF_ATOMIC_SUB_RLX(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_SUB_ACQ(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_SUB_REL(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_SUB_ARL(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_SUB_CST(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-bitwise-and
#define __MCF_ATOMIC_AND_RLX(m,r)   __atomic_fetch_and(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_AND_ACQ(m,r)   __atomic_fetch_and(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_AND_REL(m,r)   __atomic_fetch_and(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_AND_ARL(m,r)   __atomic_fetch_and(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_AND_CST(m,r)   __atomic_fetch_and(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-bitwise-or
#define __MCF_ATOMIC_OR_RLX(m,r)   __atomic_fetch_or(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_OR_ACQ(m,r)   __atomic_fetch_or(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_OR_REL(m,r)   __atomic_fetch_or(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_OR_ARL(m,r)   __atomic_fetch_or(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_OR_CST(m,r)   __atomic_fetch_or(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-bitwise-xor
#define __MCF_ATOMIC_XOR_RLX(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XOR_ACQ(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XOR_REL(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XOR_ARL(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XOR_CST(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_SEQ_CST)

#endif  // __MCFGTHREAD_ATOMIC_H_
