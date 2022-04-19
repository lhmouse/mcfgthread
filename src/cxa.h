/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_CXA_H_
#define __MCFGTHREAD_CXA_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_CXA_EXTERN_INLINE
#  define __MCF_CXA_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* Declare functions in accordance with the Itanium ABI. They are aliases for
 * the ones with the `__MCF_` prefix.
 *
 * See <https://itanium-cxx-abi.github.io/cxx-abi/abi.html> for details about
 * individual functions.

 * Note: In the case of i386, the argument is passed both via the ECX register
 * and on the stack, to allow both `__cdecl` and `__thiscall` functions to work
 * properly. The function prototype is declared for compatibility with GCC.  */
typedef void __cdecl __MCF_cxa_dtor_cdecl(void* __this);
typedef void __thiscall __MCF_cxa_dtor_thiscall(void* __this);

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
  }
  typedef __MCF_cxa_dtor_union;

typedef void __MCF_atexit_callback(void);

/* Declare 'real' functions here.  */
int
__MCF_cxa_guard_acquire(int64_t* __guard) __MCF_NOEXCEPT;

void
__MCF_cxa_guard_release(int64_t* __guard) __MCF_NOEXCEPT;

void
__MCF_cxa_guard_abort(int64_t* __guard) __MCF_NOEXCEPT;

int
__MCF_cxa_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

int
__MCF_atexit(__MCF_atexit_callback __atfn) __MCF_NOEXCEPT;

int
__MCF_cxa_at_quick_exit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

int
__MCF_at_quick_exit(__MCF_atexit_callback __atfn) __MCF_NOEXCEPT;

int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

int
__MCF_thread_atexit(__MCF_atexit_callback __atfn) __MCF_NOEXCEPT;

void
__MCF_cxa_finalize(void* __dso) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_CXA_H_  */
