/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_CXA_
#define __MCFGTHREAD_CXA_

#include "fwd.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_CXA_IMPORT
#  define __MCF_CXA_IMPORT
#  define __MCF_CXA_INLINE  __MCF_GNU_INLINE
#endif

/* See <https://itanium-cxx-abi.github.io/cxx-abi/abi.html> for details about
 * individual functions.  */

__MCF_CXA_IMPORT
int
__MCF_cxa_guard_acquire(int64_t* __guard) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
void
__MCF_cxa_guard_release(int64_t* __guard) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
void
__MCF_cxa_guard_abort(int64_t* __guard) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
int
__MCF_cxa_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
int
__MCF_atexit(__MCF_atexit_callback __atfn) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
int
__MCF_cxa_at_quick_exit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
int
__MCF_at_quick_exit(__MCF_atexit_callback __atfn) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
int
__MCF_thread_atexit(__MCF_atexit_callback __atfn) __MCF_NOEXCEPT;

__MCF_CXA_IMPORT
void
__MCF_cxa_finalize(void* __dso) __MCF_NOEXCEPT;

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_CXA_  */
