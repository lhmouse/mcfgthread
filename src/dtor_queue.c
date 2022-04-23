/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_DTOR_QUEUE_EXTERN_INLINE
#include "dtor_queue.h"
#include "memory.h"
#include "mutex.h"
#include "win32.h"

int
__MCF_dtor_queue_push(__MCF_dtor_queue* queue, const __MCF_dtor_element* elem)
  {
    if(queue->__size == __MCF_DTOR_QUEUE_BLOCK_SIZE) {
      /* If the current block is full, allocate a new one and create a singly
       * linked list.  */
      __MCF_dtor_queue* prev = _MCF_malloc_copy(queue, sizeof(__MCF_dtor_queue));
      if(!prev)
        return -1;

      queue->__prev = prev;
      queue->__size = 0;
    }

    /* There is room in the current block, so append it there.  */
    __MCFGTHREAD_ASSERT(queue->__size < __MCF_DTOR_QUEUE_BLOCK_SIZE);
    __MCF_dtor_element* target = queue->__data + queue->__size;
    *target = *elem;
    queue->__size ++;
    return 0;
  }

static inline
void
do_remove_element_common(__MCF_dtor_queue* queue, size_t index)
  {
    __MCFGTHREAD_ASSERT(index < queue->__size);
    queue->__size --;
    for(size_t k = index;  k != queue->__size;  ++k)
      queue->__data[k] = queue->__data[k+1];
  }

int
__MCF_dtor_queue_pop(__MCF_dtor_element* elem, __MCF_dtor_queue* queue, void* dso)
  {
    /* Search for a matching element in the newest block.  */
    __MCF_dtor_queue* cur_q = queue;
    int err = -1;

    do {
      /* Search backwards for an element matching `dso`.  */
      for(uint32_t k = cur_q->__size - 1;  k != UINT32_MAX;  --k) {
        __MCF_dtor_element* target = cur_q->__data + k;
        if(!dso || (dso == target->__dso)) {
          /* Remove this element.  */
          *elem = *target;
          err = 0;
          do_remove_element_common(cur_q, k);
          break;
        }
      }

      /* If the current block has become empty, free it.
       * Otherwise, go to the next one.  */
      if((cur_q->__size == 0) && cur_q->__prev) {
        __MCF_dtor_queue* prev = cur_q->__prev;
        _MCF_mmove(cur_q, prev, sizeof(__MCF_dtor_queue));
        _MCF_mfree_nonnull(prev);
      }
      else
        cur_q = cur_q->__prev;
    }
    while((err != 0) && cur_q);

    return err;
  }

size_t
__MCF_dtor_queue_remove(__MCF_dtor_queue* queue, void* dso)
  {
    /* Search for a matching element in the newest block.  */
    __MCF_dtor_queue* cur_q = queue;
    size_t count = 0;

    do {
      /* Search backwards for an element matching `dso`.  */
      for(uint32_t k = cur_q->__size - 1;  k != UINT32_MAX;  --k) {
        __MCF_dtor_element* target = cur_q->__data + k;
        if(!dso || (dso == target->__dso)) {
          /* Remove this element.  */
          count ++;
          do_remove_element_common(cur_q, k);
        }
      }

      /* If the current block has become empty, free it.
       * Otherwise, go to the next one.  */
      if((cur_q->__size == 0) && cur_q->__prev) {
        __MCF_dtor_queue* prev = cur_q->__prev;
        _MCF_mmove(cur_q, prev, sizeof(__MCF_dtor_queue));
        _MCF_mfree_nonnull(prev);
      }
      else
        cur_q = cur_q->__prev;
    }
    while(cur_q);

    return count;
  }

void
__MCF_dtor_queue_finalize(__MCF_dtor_queue* queue, _MCF_mutex* mutex_opt, void* dso)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_dtor_element elem;
    int err;

    for(;;) {
      /* Try popping an element.  */
      if(mutex_opt)
        _MCF_mutex_lock(mutex_opt, NULL);

      err = __MCF_dtor_queue_pop(&elem, queue, dso);

      if(mutex_opt)
        _MCF_mutex_unlock(mutex_opt);

      if(err != 0)
        break;

#ifdef __i386__
      /* Note: In the case of i386, the argument is passed both via the ECX
       * register and on the stack, to allow both `__cdecl` and `__thiscall`
       * functions to work properly.
       * Parameters: EAX, EDX, ECX, ESP[4]  */
      typedef void __attribute__((__regparm__(3))) i386_dtor(void*, void*, void*, void*);
      (*(i386_dtor*)(intptr_t) elem.__dtor)(0, 0, elem.__this, elem.__this);
#else
      /* This works on x86_64, and should work on ARM (FIXME: untested).  */
      elem.__dtor(elem.__this);
#endif
    }
  }
