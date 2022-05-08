/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DTOR_QUEUE_EXTERN_INLINE
#include "dtor_queue.h"
#include "mutex.h"
#include "xwin32.i"

__MCF_DLLEXPORT
int
__MCF_dtor_queue_push(__MCF_dtor_queue* queue, const __MCF_dtor_element* elem)
  {
    if(queue->__size == __MCF_DTOR_QUEUE_BLOCK_SIZE) {
      /* If the current block is full, allocate a new one and create a singly
       * linked list.  */
      __MCF_dtor_queue* prev = __MCF_malloc_copy(queue, sizeof(__MCF_dtor_queue));
      if(!prev)
        return -1;

      queue->__prev = prev;
      queue->__size = 0;
    }

    /* There is room in the current block, so append it there.  */
    uint32_t index = queue->__size;
    __MCFGTHREAD_ASSERT(index < __MCF_DTOR_QUEUE_BLOCK_SIZE);
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
        err = 0;
        *elem = cur_q->__data[index];
        cur_q->__size --;

        if(index != cur_q->__size)
          __MCF_mmove(cur_q->__data + index, cur_q->__data + index + 1,
                        (cur_q->__size - index) * sizeof(__MCF_dtor_element));
      }

      /* If the current block has become empty, deallocate it. Otherwise,
       * go to the next one.  */
      __MCF_dtor_queue* prev = cur_q->__prev;
      if((cur_q->__size == 0) && prev) {
        __MCF_mcopy(cur_q, prev, sizeof(__MCF_dtor_queue));
        __MCF_mfree(prev);
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
        __MCF_mfree(prev);
      }
      else
        cur_q = prev;
    }
    while(cur_q);

    return count;
  }

__MCF_DLLEXPORT
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
