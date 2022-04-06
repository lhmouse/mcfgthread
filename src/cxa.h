// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_CXA_H_
#define __MCFGTHREAD_CXA_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCFGTHREAD_CXA_C_
#  define __MCFGTHREAD_CXA_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_CXA_INLINE
#endif

// Declare functions in accordance with the Itanium ABI. An inline wrapper with
// a prefixed name is also provided for each of them.
//
// See <https://itanium-cxx-abi.github.io/cxx-abi/abi.html> for details about
// individual functions.
//
// Note: In the case of i386, the argument is passed both via the ECX register
// and on the stack, to allow both `__cdecl` and `__thiscall` functions to work
// properly. The function prototype is declared for compatibility with GCC.
typedef void __thiscall __MCF_cxa_dtor_thiscall(void* __this);
typedef void __cdecl __MCF_cxa_dtor_cdecl(void* __this);

union __attribute__((__transparent_union__)) __MCF_cxa_dtor_union
  {
    __MCF_cxa_dtor_thiscall* __thiscall_ptr;
    __MCF_cxa_dtor_cdecl* __cdecl_ptr;
  }
  typedef __MCF_cxa_dtor_union;

// 3.3.3 One-time Construction API
int
__cxa_guard_acquire(int64_t* __guard) __MCF_NOEXCEPT;

int
__MCF_cxa_guard_acquire(int64_t* __guard) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE int
__cxa_guard_acquire(int64_t* __guard) __MCF_NOEXCEPT
  {
    return __MCF_cxa_guard_acquire(__guard);
  }

void
__cxa_guard_release(int64_t* __guard) __MCF_NOEXCEPT;

void
__MCF_cxa_guard_release(int64_t* __guard) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE void
__cxa_guard_release(int64_t* __guard) __MCF_NOEXCEPT
  {
    __MCF_cxa_guard_release(__guard);
  }

void
__cxa_guard_abort(int64_t* __guard) __MCF_NOEXCEPT;

void
__MCF_cxa_guard_abort(int64_t* __guard) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE void
__cxa_guard_abort(int64_t* __guard) __MCF_NOEXCEPT
  {
    __MCF_cxa_guard_abort(__guard);
  }

// 3.3.6.3 Runtime API
int
__cxa_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

int
__MCF_cxa_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE int
__cxa_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT
  {
    return __MCF_cxa_atexit(__dtor, __this, __dso);
  }

void
__cxa_finalize(void* __dso) __MCF_NOEXCEPT;

void
__MCF_cxa_finalize(void* __dso) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE void
__cxa_finalize(void* __dso) __MCF_NOEXCEPT
  {
    __MCF_cxa_finalize(__dso);
  }

// This seems a GNU extension?
int
__cxa_thread_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE int
__cxa_thread_atexit(__MCF_cxa_dtor_union __dtor, void* __this, void* __dso) __MCF_NOEXCEPT
  {
    return __MCF_cxa_thread_atexit(__dtor, __this, __dso);
  }

void
__cxa_thread_finalize(void* __dso) __MCF_NOEXCEPT;

void
__MCF_cxa_thread_finalize(void* __dso) __MCF_NOEXCEPT;

__MCFGTHREAD_CXA_INLINE void
__cxa_thread_finalize(void* __dso) __MCF_NOEXCEPT
  {
    __MCF_cxa_thread_finalize(__dso);
  }

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_CXA_H_
