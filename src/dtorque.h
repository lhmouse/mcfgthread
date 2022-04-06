// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_DTORQUE_H_
#define __MCFGTHREAD_DTORQUE_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCFGTHREAD_DTORQUE_C_
#  define __MCFGTHREAD_DTORQUE_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_DTORQUE_INLINE
#endif

// Define the cxa_atexit queue.
//
// Note: In the case of i386, the argument is passed both via the ECX register
// and on the stack, to allow both `__cdecl` and `__thiscall` functions to work
// properly. The function prototype is declared for compatibility with GCC.
typedef void __thiscall __MCF_dtor_generic(void* __this, ...);

struct __MCF_dtorelem
  {
    __MCF_dtor_generic* __dtor;
    void* __this;
    void* __dso;
  }
  typedef __MCF_dtorelem;

struct __MCF_dtorque
  {
    __MCF_dtorque* __prev;
    uint32_t __size;

#define __MCF_DTORQUE_BLOCK_SIZE  63
    __MCF_dtorelem __data[__MCF_DTORQUE_BLOCK_SIZE];
  }
  typedef __MCF_dtorque;

// Appends an element to the queue. `__elem->__dtor` shall not be null. This
// function is used to implement `__cxa_atexit()`. Refer to the Itanium C++
// ABI for details about DSO handles.
//
// Returns 0 if an element has been pushed, or -1 if out of memory.
int
__MCF_dtorque_push(__MCF_dtorque* __queue, const __MCF_dtorelem* __elem) __MCF_NOEXCEPT;

// Pops the newest element which matches `__dso` from the queue. If `__dso` is
// null, then any element is considered a match. This function is used to
// implement `__cxa_finalize()`. Refer to the Itanium C++ ABI for details about
// DSO handles.
//
// Returns 0 if an element has been popped, or -1 if the queue is empty.
int
__MCF_dtorque_pop(__MCF_dtorelem* __elem, __MCF_dtorque* __queue, void* __dso_opt) __MCF_NOEXCEPT;

// Clears the queue.
void
__MCF_dtorque_clear(__MCF_dtorque* __queue) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_DTORQUE_H_
