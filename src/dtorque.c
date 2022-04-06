// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_DTORQUE_C_  1
#include "dtorque.h"
#include "memory.h"
#include "win32.h"

int
__MCF_dtorque_push(__MCF_dtorque* queue, const __MCF_dtorelem* elem)
  {
    // Check whether the current block is full.
    if(queue->__size == __MCF_DTORQUE_BLOCK_SIZE) {
      __MCF_dtorque* prev = _MCF_malloc0(sizeof(*prev));
      if(!prev)
        return -1;

      // Move the contents of `queue` into it to create a linked list.
      _MCF_mmove(prev, queue, sizeof(*prev));
      queue->__prev = prev;
      queue->__size = 0;
    }

    // There is room in the current block, so append it there.
    __MCFGTHREAD_ASSERT(queue->__size < __MCF_DTORQUE_BLOCK_SIZE);
    queue->__data[queue->__size] = *elem;
    queue->__size ++;
    return 0;
  }

int
__MCF_dtorque_pop(__MCF_dtorelem* elem, __MCF_dtorque* queue, void* dso_opt)
  {
    // Search for a matching element in the newest block.
    __MCF_dtorque* cur_q = queue;
    int err = -1;

    do {
      // Search backwards for an element matching `dso_opt`.
      for(uint32_t k = cur_q->__size - 1;  k != UINT32_MAX;  --k) {
        if((dso_opt == NULL) || (dso_opt == cur_q->__data[k].__dso)) {
          *elem = cur_q->__data[k];
          err = 0;

          // Remove this element.
          for(size_t r = k;  r != cur_q->__size - 1;  ++r)
            cur_q->__data[r] = cur_q->__data[r + 1];

          cur_q->__size --;
          break;
        }
      }

      // If the current block has become empty, free it.
      // Otherwise, go to the next one.
      if((cur_q->__size == 0) && cur_q->__prev) {
        __MCF_dtorque* prev = cur_q->__prev;
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
__MCF_dtorque_clear(__MCF_dtorque* queue)
  {
    // Free all blocks other than `queue`.
    while(queue->__prev) {
      __MCF_dtorque* prev = queue->__prev;
      queue->__prev = prev->__prev;
      _MCF_mfree(prev);
    }

    // Truncate `queue` as there is no longer dynamic storage.
    queue->__size = 0;
  }
