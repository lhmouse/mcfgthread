// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_DTOR_QUEUE_C_  1
#include "dtor_queue.h"
#include "memory.h"
#include "win32.h"

int
__MCF_dtor_queue_push(__MCF_dtor_queue* queue, const __MCF_dtor_element* elem)
  {
    // Check whether the current block is full.
    if(queue->__size == __MCF_DTOR_QUEUE_BLOCK_SIZE) {
      __MCF_dtor_queue* prev = _MCF_malloc0(sizeof(*prev));
      if(!prev)
        return -1;

      // Move the contents of `queue` into it to create a linked list.
      _MCF_mmove(prev, queue, sizeof(*prev));
      queue->__prev = prev;
      queue->__size = 0;
    }

    // There is room in the current block, so append it there.
    __MCFGTHREAD_ASSERT(queue->__size < __MCF_DTOR_QUEUE_BLOCK_SIZE);
    __MCF_dtor_element* target = queue->__data + queue->__size;
    *target = *elem;
    queue->__size ++;
    return 0;
  }

int
__MCF_dtor_queue_pop(__MCF_dtor_element* elem, __MCF_dtor_queue* queue, void* dso_opt)
  {
    // Search for a matching element in the newest block.
    __MCF_dtor_queue* cur_q = queue;
    int err = -1;

    do {
      // Search backwards for an element matching `dso_opt`.
      for(uint32_t k = cur_q->__size - 1;  k != UINT32_MAX;  --k) {
        __MCF_dtor_element* target = cur_q->__data + k;
        if(!dso_opt || (dso_opt == target->__dso)) {
          *elem = *target;
          err = 0;

          // Remove this element.
          cur_q->__size --;
          _MCF_mmove(target, target + 1, (cur_q->__size - k) * sizeof(*elem));
          break;
        }
      }

      // If the current block has become empty, free it.
      // Otherwise, go to the next one.
      if((cur_q->__size == 0) && cur_q->__prev) {
        __MCF_dtor_queue* prev = cur_q->__prev;
        _MCF_mmove(cur_q, prev, sizeof(*prev));
        _MCF_mfree(prev);
      }
      else
        cur_q = cur_q->__prev;
    }
    while((err != 0) && cur_q);

    return err;
  }

void
__MCF_dtor_queue_clear(__MCF_dtor_queue* queue)
  {
    // Free all blocks other than `queue`.
    while(queue->__prev) {
      __MCF_dtor_queue* prev = queue->__prev;
      queue->__prev = prev->__prev;
      _MCF_mfree(prev);
    }

    // Truncate `queue` as there is no longer dynamic storage.
    queue->__size = 0;
  }
