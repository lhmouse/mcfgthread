/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_DTOR_QUEUE_IMPORT  __MCF_DLLEXPORT
#define __MCF_DTOR_QUEUE_INLINE  __MCF_DLLEXPORT
#include "../dtor_queue.h"
#include "../mutex.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
__MCF_dtor_queue_reserve_and_push(__MCF_dtor_queue* queue, uint32_t reservation,
                                  const __MCF_dtor_element* elem)
  {
    uint32_t capacity = ARRAYSIZE(queue->__data);
    if((reservation < 1) || (reservation > capacity) || !elem || !elem->__dtor)
      return -1;

    if(reservation > capacity - queue->__size) {
      __MCF_dtor_queue* prev = __MCF_malloc_copy(queue, sizeof(__MCF_dtor_queue));
      if(!prev)
        return -2;

      /* Create a singly-linked list by moving elements to `*prev`. The
       * current block is empty now.  */
      queue->__prev = prev;
      queue->__size = 0;
    }

    /* There is room in the current block, so append it there.  */
    uint32_t index = queue->__size;
    __MCF_ASSERT(index < capacity);
    queue->__data[index] = *elem;
    queue->__size ++;
    return 0;
  }

__MCF_DLLEXPORT
int
__MCF_dtor_queue_push(__MCF_dtor_queue* queue, const __MCF_dtor_element* elem)
  {
    return __MCF_dtor_queue_reserve_and_push(queue, 1, elem);
  }

static
size_t
do_pop_element_common(__MCF_dtor_element* out_elem_opt, __MCF_dtor_queue* queue, void* dso)
  {
    size_t npopped = 0;
    __MCF_dtor_queue* cur_queue = queue;

  try_cur_queue:
    if(out_elem_opt) {
      /* Search backwards for an element matching `dso`.  */
      uint32_t index = cur_queue->__size - 1;
      if(dso)
        while((index != UINT32_MAX) && (dso != cur_queue->__data[index].__dso))
          index --;

      if(index != UINT32_MAX) {
        /* Found, so copy it to `*out_elem_opt`.  */
        *out_elem_opt = cur_queue->__data[index];
        npopped ++;
        cur_queue->__size --;

        if(index != cur_queue->__size)
          __MCF_mcopy(cur_queue->__data + index, cur_queue->__data + index + 1,
                      (cur_queue->__size - index) * sizeof(__MCF_dtor_element));

        /* In this case, only one element can be popped.  */
        __MCF_ASSERT(npopped == 1);
        return npopped;
      }
    }
    else {
      /* Search forwards and copy all elements not matching `dso`.  */
      uint32_t index = UINT32_MAX, new_size = 0;
      if(dso)
        while(++ index != cur_queue->__size)
          if(dso != cur_queue->__data[index].__dso)
            cur_queue->__data[new_size ++] = cur_queue->__data[index];

      npopped += cur_queue->__size - new_size;
      cur_queue->__size = new_size;
    }

    /* The current queue has been exhausted. If it is empty now, it should be
     * deallocated. If there are more than one blocks, go to the previous one.  */
    __MCF_dtor_queue* prev = cur_queue->__prev;
    if(prev) {
      if(cur_queue->__size != 0)
        cur_queue = prev;
      else {
        __MCF_mcopy(cur_queue, prev, sizeof(__MCF_dtor_queue));
        __MCF_mfree_nonnull(prev);
      }

      /* Look for more elements in the previous block.  */
      goto try_cur_queue;
    }

    /* Return the number of elements that have been popped. When `out_elem_opt`
     * is null and a matching element is found, this function returns early so
     * here it can only return zero.  */
    __MCF_ASSERT(!out_elem_opt || (npopped == 0));
    return npopped;
  }

__MCF_DLLEXPORT
int
__MCF_dtor_queue_pop(__MCF_dtor_element* out_elem, __MCF_dtor_queue* queue, void* dso)
  {
    size_t npopped = do_pop_element_common(out_elem, queue, dso);
    __MCF_ASSERT(npopped <= 1);
    return (int) npopped - 1;
  }

__MCF_DLLEXPORT
size_t
__MCF_dtor_queue_remove(__MCF_dtor_queue* queue, void* dso)
  {
    return do_pop_element_common(nullptr, queue, dso);
  }
