// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_DTOR_QUEUE_H_
#define __MCFGTHREAD_DTOR_QUEUE_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MCFGTHREAD_DTOR_QUEUE_C_
#  define __MCFGTHREAD_DTOR_QUEUE_INLINE
#else
#  define __MCFGTHREAD_DTOR_QUEUE_INLINE  __MCF_GNU_INLINE
#endif

// Note: In the case of i386, the argument is passed both via the ECX register
// and on the stack, to allow both `__cdecl` and `__thiscall` functions to work
// properly. The function prototype is declared according to the Itanium ABI.
typedef void __cdecl __MCF_cxa_dtor_cdecl(void* __this);

// Define the cxa_atexit queue structure.
struct __MCF_dtor_element
  {
    __MCF_cxa_dtor_cdecl* __dtor;
    void* __this;
    void* __dso;
  }
  typedef __MCF_dtor_element;

struct __MCF_dtor_queue
  {
    __MCF_dtor_queue* __prev;
    uint32_t __size;

#define __MCF_DTOR_QUEUE_BLOCK_SIZE  63U
    __MCF_dtor_element __data[__MCF_DTOR_QUEUE_BLOCK_SIZE];
  }
  typedef __MCF_dtor_queue;

// Appends an element to the queue. `__elem->__dtor` shall not be null. This
// function is used to implement `__cxa_atexit()`. Refer to the Itanium C++
// ABI for details about DSO handles.
//
// Returns 0 if an element has been pushed, or -1 if out of memory.
int
__MCF_dtor_queue_push(__MCF_dtor_queue* __queue, const __MCF_dtor_element* __elem) __MCF_NOEXCEPT;

// Pops the newest element which matches `__dso` from the queue. If `__dso` is
// null, then any element is considered a match. This function is used to
// implement `__cxa_finalize()`. Refer to the Itanium C++ ABI for details about
// DSO handles.
//
// Returns 0 if an element has been popped, or -1 if the queue is empty.
int
__MCF_dtor_queue_pop(__MCF_dtor_element* __elem, __MCF_dtor_queue* __queue, void* __dso) __MCF_NOEXCEPT;

// Executes all destructors which match `__dso` in the queue. If `__dso` is
// null, then all elements are considered matches. This function is used to
// implement `__cxa_finalize()`. Refer to the Itanium C++ ABI for details about
// DSO handles.
void
__MCF_dtor_queue_finalize(__MCF_dtor_queue* __queue, _MCF_mutex* __mutex_opt, void* __dso) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_DTOR_QUEUE_H_
