/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_DTOR_QUEUE_IMPORT  __MCF_DLLEXPORT
#define __MCF_DTOR_QUEUE_INLINE  __MCF_DLLEXPORT
#include "dtor_queue.h"
#include "mutex.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
__MCF_dtor_queue_push(__MCF_dtor_queue* queue, const __MCF_dtor_element* elem)
  {
    if(queue->__size == __MCF_DTOR_QUEUE_BLOCK_SIZE) {
      __MCF_dtor_queue* prev = __MCF_malloc_copy(queue, sizeof(__MCF_dtor_queue));
      if(!prev)
        return __MCF_win32_error_i(ERROR_NOT_ENOUGH_MEMORY, -1);

      /* Create a singly-linked list by moving elements to `*prev`. The
       * current block is empty now.  */
      queue->__prev = prev;
      queue->__size = 0;
    }

    /* There is room in the current block, so append it there.  */
    uint32_t index = queue->__size;
    __MCF_ASSERT(index < __MCF_DTOR_QUEUE_BLOCK_SIZE);
    queue->__data[index] = *elem;
    queue->__size ++;
    return 0;
  }

__MCF_DLLEXPORT
int
__MCF_dtor_queue_pop(__MCF_dtor_element* elem, __MCF_dtor_queue* queue, void* dso)
  {
    /* Search for a matching element in the newest block.  */
    __MCF_dtor_queue* cur_q = queue;
    int err = -1;

    do {
      uint32_t index = cur_q->__size - 1;

      /* Search backwards for an element matching `dso`.  */
      if(dso)
        while((index != UINT32_MAX) && (dso != cur_q->__data[index].__dso))
          index --;

      /* If an element has been found, remove it.  */
      if(index != UINT32_MAX) {
        __MCF_dtor_element* cur_elem = cur_q->__data + index;
        *elem = *cur_elem;
        cur_q->__size --;
        err = 0;

        uint32_t shift = cur_q->__size - index;
        if(shift != 0)
          __MCF_mcopy(cur_elem, cur_elem + 1, shift * sizeof(__MCF_dtor_element));
      }

      /* If the current block has become empty, deallocate it. Otherwise,
       * go to the next one.  */
      __MCF_dtor_queue* prev = cur_q->__prev;
      if((cur_q->__size == 0) && prev) {
        __MCF_mcopy(cur_q, prev, sizeof(__MCF_dtor_queue));
        __MCF_mfree_nonnull(prev);
      }
      else
        cur_q = prev;
    }
    while((err != 0) && cur_q);

    return err;
  }

__MCF_DLLEXPORT
size_t
__MCF_dtor_queue_remove(__MCF_dtor_queue* queue, void* dso)
  {
    /* Search for a matching element in the newest block.  */
    __MCF_dtor_queue* cur_q = queue;
    size_t count = 0;

    do {
      uint32_t index = UINT32_MAX;
      uint32_t new_size = 0;

      /* Search forwards and copy all elements not matching `dso`.  */
      if(dso)
        while(++index != cur_q->__size)
          if(dso != cur_q->__data[index].__dso)
            cur_q->__data[new_size++] = cur_q->__data[index];

      /* Truncate the current block.  */
      count += cur_q->__size - new_size;
      cur_q->__size = new_size;

      /* If the current block has become empty, deallocate it. Otherwise,
       * go to the next one.  */
      __MCF_dtor_queue* prev = cur_q->__prev;
      if((cur_q->__size == 0) && prev) {
        __MCF_mcopy(cur_q, prev, sizeof(__MCF_dtor_queue));
        __MCF_mfree_nonnull(prev);
      }
      else
        cur_q = prev;
    }
    while(cur_q);

    return count;
  }
