/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_CXA_
#define __MCFGTHREAD_CXA_

#include "fwd.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_CXA_IMPORT
#  define __MCF_CXA_IMPORT
#  define __MCF_CXA_INLINE  __MCF_GNU_INLINE
#endif

/* See <https://itanium-cxx-abi.github.io/cxx-abi/abi.html> for details about
 * individual functions.  */
__MCF_CXA_IMPORT
int
__MCF_cxa_guard_acquire(int64_t* __guard) __MCF_noexcept;

__MCF_CXA_IMPORT
void
__MCF_cxa_guard_release(int64_t* __guard) __MCF_noexcept;

__MCF_CXA_IMPORT
void
__MCF_cxa_guard_abort(int64_t* __guard) __MCF_noexcept;

__MCF_CXA_IMPORT
int
__MCF_cxa_atexit(__MCF_cxa_dtor_any_ __dtor, void* __this, void* __dso) __MCF_noexcept;

__MCF_CXA_IMPORT
int
__MCF_atexit(__MCF_atexit_callback* __func) __MCF_noexcept;

__MCF_CXA_IMPORT
int
__MCF_cxa_at_quick_exit(__MCF_cxa_dtor_any_ __dtor, void* __this, void* __dso) __MCF_noexcept;

__MCF_CXA_IMPORT
int
__MCF_at_quick_exit(__MCF_atexit_callback* __func) __MCF_noexcept;

__MCF_CXA_IMPORT
int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_any_ __dtor, void* __this, void* __dso) __MCF_noexcept;

__MCF_CXA_IMPORT
int
__MCF_thread_atexit(__MCF_atexit_callback* __func) __MCF_noexcept;

__MCF_CXA_IMPORT
void
__MCF_cxa_finalize(void* __dso) __MCF_noexcept;

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_CXA_  */
