/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_DTOR_QUEUE_
#define __MCFGTHREAD_DTOR_QUEUE_

#include "fwd.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_DTOR_QUEUE_IMPORT
#  define __MCF_DTOR_QUEUE_IMPORT
#  define __MCF_DTOR_QUEUE_INLINE  __MCF_GNU_INLINE
#endif

/* Define the cxa_atexit queue structure.  */
struct __MCF_dtor_element
  {
    __MCF_cxa_dtor_cdecl* __dtor;
    void* __this;
    void* __dso;
  };

struct __MCF_dtor_queue
  {
    __MCF_dtor_queue* __prev;
    uint32_t __size;

#define __MCF_DTOR_QUEUE_BLOCK_SIZE  63U
    __MCF_dtor_element __data[__MCF_DTOR_QUEUE_BLOCK_SIZE];
  };

/* Appends an element to the queue. `__elem->__dtor` shall not be null. This
 * function is used to implement `__cxa_atexit()`. Refer to the Itanium C++
 * ABI for details about DSO handles.
 *
 * Returns 0 if an element has been pushed, or -1 if out of memory.  */
__MCF_DTOR_QUEUE_IMPORT
int
__MCF_dtor_queue_push(__MCF_dtor_queue* __queue, const __MCF_dtor_element* __elem) __MCF_NOEXCEPT;

/* Pops the newest element which matches `__dso` from the queue. If `__dso` is
 * null, then any element is considered a match. This function is used to
 * implement `__cxa_finalize()`. Refer to the Itanium C++ ABI for details about
 * DSO handles.
 *
 * Returns 0 if an element has been popped, or -1 if the queue is empty.  */
__MCF_DTOR_QUEUE_IMPORT
int
__MCF_dtor_queue_pop(__MCF_dtor_element* __elem, __MCF_dtor_queue* __queue, void* __dso) __MCF_NOEXCEPT;

/* Removes all elements that match `__dso` from the queue. If `__dso` is null,
 * then any element is considered a match. Refer to the Itanium C++ ABI for
 * details about DSO handles.
 *
 * Returns the number of elements that have been removed.  */
__MCF_DTOR_QUEUE_IMPORT
size_t
__MCF_dtor_queue_remove(__MCF_dtor_queue* __queue, void* __dso) __MCF_NOEXCEPT;

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_DTOR_QUEUE_  */
