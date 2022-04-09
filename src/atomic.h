// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_ATOMIC_H_
#define __MCFGTHREAD_ATOMIC_H_

#include "fwd.h"

// Generic atomic load
// Note the order of arguments is swapped. Output precedes input.
#define __MCF_ATOMIC_LOAD_RELAXED(p,m)   __atomic_load(m, p, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_LOAD_ACQUIRE(p,m)   __atomic_load(m, p, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_LOAD_SEQ_CST(p,m)   __atomic_load(m, p, __ATOMIC_SEQ_CST)

// Scalar atomic load
#define __MCF_ATOMIC_LOAD_N_RELAXED(m)   __atomic_load_n(m, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_LOAD_N_ACQUIRE(m)   __atomic_load_n(m, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_LOAD_N_SEQ_CST(m)   __atomic_load_n(m, __ATOMIC_SEQ_CST)

// Generic atomic store
#define __MCF_ATOMIC_STORE_RELAXED(m,p)   __atomic_store(m, p, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_STORE_RELEASE(m,p)   __atomic_store(m, p, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_STORE_SEQ_CST(m,p)   __atomic_store(m, p, __ATOMIC_SEQ_CST)

// Scalar atomic store
#define __MCF_ATOMIC_STORE_N_RELAXED(m,r)   __atomic_store_n(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_STORE_N_RELEASE(m,r)   __atomic_store_n(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_STORE_N_SEQ_CST(m,r)   __atomic_store_n(m, r, __ATOMIC_SEQ_CST)

// Generic atomic swap
// Note the order of arguments is swapped. Output precedes input.
#define __MCF_ATOMIC_SWAP_RELAXED(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_SWAP_ACQUIRE(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_SWAP_RELEASE(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_SWAP_ACQ_REL(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_SWAP_SEQ_CST(p,m,r)   __atomic_exchange(m, r, p, __ATOMIC_SEQ_CST)

// Scalar atomic swap
#define __MCF_ATOMIC_SWAP_N_RELAXED(m,r)   __atomic_exchange_n(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_SWAP_N_ACQUIRE(m,r)   __atomic_exchange_n(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_SWAP_N_RELEASE(m,r)   __atomic_exchange_n(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_SWAP_N_ACQ_REL(m,r)   __atomic_exchange_n(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_SWAP_N_SEQ_CST(m,r)   __atomic_exchange_n(m, r, __ATOMIC_SEQ_CST)

// Generic strong compare-and-swap
#define __MCF_ATOMIC_CAS_RELAXED(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CAS_ACQUIRE(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CAS_RELEASE(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CAS_ACQ_REL(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CAS_SEQ_CST(m,c,p)   __atomic_compare_exchange(m, c, p, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Scalar strong compare-and-swap
#define __MCF_ATOMIC_CAS_N_RELAXED(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CAS_N_ACQUIRE(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CAS_N_RELEASE(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_CAS_N_ACQ_REL(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_CAS_N_SEQ_CST(m,c,r)   __atomic_compare_exchange_n(m, c, r, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Generic weak compare-and-swap
#define __MCF_ATOMIC_WCAS_RELAXED(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_WCAS_ACQUIRE(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_WCAS_RELEASE(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_WCAS_ACQ_REL(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_WCAS_SEQ_CST(m,c,p)   __atomic_compare_exchange(m, c, p, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Scalar weak compare-and-swap
#define __MCF_ATOMIC_WCAS_N_RELAXED(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_WCAS_N_ACQUIRE(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_WCAS_N_RELEASE(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_WCAS_N_ACQ_REL(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_WCAS_N_SEQ_CST(m,c,r)   __atomic_compare_exchange_n(m, c, r, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// Scalar exchange-and-add
#define __MCF_ATOMIC_ADD_RELAXED(m,r)   __atomic_fetch_add(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_ADD_ACQUIRE(m,r)   __atomic_fetch_add(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_ADD_RELEASE(m,r)   __atomic_fetch_add(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_ADD_ACQ_REL(m,r)   __atomic_fetch_add(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_ADD_SEQ_CST(m,r)   __atomic_fetch_add(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-sub
#define __MCF_ATOMIC_SUB_RELAXED(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_SUB_ACQUIRE(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_SUB_RELEASE(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_SUB_ACQ_REL(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_SUB_SEQ_CST(m,r)   __atomic_fetch_sub(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-bitwise-and
#define __MCF_ATOMIC_AND_RELAXED(m,r)   __atomic_fetch_and(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_AND_ACQUIRE(m,r)   __atomic_fetch_and(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_AND_RELEASE(m,r)   __atomic_fetch_and(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_AND_ACQ_REL(m,r)   __atomic_fetch_and(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_AND_SEQ_CST(m,r)   __atomic_fetch_and(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-bitwise-or
#define __MCF_ATOMIC_OR_RELAXED(m,r)   __atomic_fetch_or(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_OR_ACQUIRE(m,r)   __atomic_fetch_or(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_OR_RELEASE(m,r)   __atomic_fetch_or(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_OR_ACQ_REL(m,r)   __atomic_fetch_or(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_OR_SEQ_CST(m,r)   __atomic_fetch_or(m, r, __ATOMIC_SEQ_CST)

// Scalar exchange-and-bitwise-xor
#define __MCF_ATOMIC_XOR_RELAXED(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_RELAXED)
#define __MCF_ATOMIC_XOR_ACQUIRE(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_ACQUIRE)
#define __MCF_ATOMIC_XOR_RELEASE(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_RELEASE)
#define __MCF_ATOMIC_XOR_ACQ_REL(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_ACQ_REL)
#define __MCF_ATOMIC_XOR_SEQ_CST(m,r)   __atomic_fetch_xor(m, r, __ATOMIC_SEQ_CST)

#endif  // __MCFGTHREAD_ATOMIC_H_
