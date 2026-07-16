/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_DTOR_QUEUE_
#define __MCFGTHREAD_DTOR_QUEUE_

#include "fwd.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_DTOR_QUEUE_IMPORT
#  define __MCF_DTOR_QUEUE_IMPORT
#  define __MCF_DTOR_QUEUE_INLINE  __MCF_GNU_INLINE
#endif

/* This structure contains one registered callback.  */
struct __MCF_dtor_element
  {
    __MCF_cxa_dtor_cdecl* __dtor;
    void* __this;
    void* __dso;
  };

/* A destructor queue is a singly-linked list of blocks of callbacks.
 *
 * The queue itself provides storage for the initial 63 callbacks without
 * allocation, to meet the requirement of the C standard, which specifies that
 * the first 32 calls to `atexit()` or `at_quick_exit()` shall not fail.  */
struct __MCF_dtor_queue
  {
    __MCF_dtor_queue* __prev;
    uint32_t __size;
    __MCF_dtor_element __data[63];
  };

/* Appends an element to the end of a destructor queue.
 *
 * @param `queue` points to a queue where the element will be appended.
 * @param `elem` points to an element to be appended. `elem->__dtor` shall not
 *     be a null pointer.
 * @returns 0 if the element has been pushed successfully, -1 in case of an
 *     invalid argument, or -2 if out of memory.  */
__MCF_DTOR_QUEUE_IMPORT
int
__MCF_dtor_queue_push(__MCF_dtor_queue* __queue, const __MCF_dtor_element* __elem)
  __MCF_noexcept;

/* Pops the newest element that matches a DSO from a destructor queue.
 *
 * @param `elem` points to a structure which will receive the popped element.
 * @param `queue` points to a queue where an element will be popped.
 * @param `dso` is an optional DSO handle. If `dso` is null, then the newest
 *     element will be popped, regardless of its DSO handle; otherwise, only
 *     the newest element whose `__dso` field equals `dso` will be popped.
 * @returns 0 if an element has been popped, or -1 if there's no more matching
 *     element in the queue.  */
__MCF_DTOR_QUEUE_IMPORT
int
__MCF_dtor_queue_pop(__MCF_dtor_element* __elem, __MCF_dtor_queue* __queue, void* __dso)
  __MCF_noexcept;

/* Removes all elements that match a DSO from a destructor queue.
 *
 * @param `queue` points to a queue where elements will be removed.
 * @param `dso` is an optional DSO handle. If `dso` is null, then all elements
 *     will be removed, regardless of their DSO handles; otherwise, only the
 *     elements whose `__dso` field equals `dso` will be removed.
 * @returns the number of elements that have been removed.  */
__MCF_DTOR_QUEUE_IMPORT
size_t
__MCF_dtor_queue_remove(__MCF_dtor_queue* __queue, void* __dso)
  __MCF_noexcept;

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_DTOR_QUEUE_  */
