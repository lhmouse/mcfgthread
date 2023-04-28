/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

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

union __attribute__((__transparent_union__)) __MCF_cxa_dtor_union
  {
    __MCF_cxa_dtor_cdecl* __cdecl_ptr;
    __MCF_cxa_dtor_thiscall* __thiscall_ptr;

#ifdef __cplusplus
    /* GCC ignores `__transparent_union__` attribute so mimic it.  */
    __MCF_CXX11(constexpr)
    __MCF_cxa_dtor_union(__MCF_cxa_dtor_cdecl* __arg) __MCF_NOEXCEPT
      : __cdecl_ptr(__arg)  { }

#  ifdef __i386__
    __MCF_CXX11(constexpr)
    __MCF_cxa_dtor_union(__MCF_cxa_dtor_thiscall* __arg) __MCF_NOEXCEPT
      : __thiscall_ptr(__arg)  { }
#  endif
#endif  /* __cplusplus  */
  };

/* Declare 'real' functions here.  */
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
