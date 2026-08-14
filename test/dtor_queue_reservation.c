/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/dtor_queue.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __MCF_dtor_queue queue;

int
main(void)
  {
    __MCF_dtor_element elem = { 0 };
    int r;

    for(intptr_t k = 0;  k <= 64;  ++k) {
      elem.__dtor = (__MCF_cxa_dtor_cdecl*) k;
      elem.__dso = (void*) (k % 10);

      r = __MCF_dtor_queue_reserve_and_push(&queue, (uint32_t) k, &elem);
      if((k < 1) || (k > 63))
        assert(r == -1);
      else
        assert(r == 0);
    }

    for(intptr_t k = 1;  k <= 1000;  ++k) {
      elem.__dtor = (__MCF_cxa_dtor_cdecl*) k;
      elem.__dso = (void*) (k % 10);

      r = __MCF_dtor_queue_reserve_and_push(&queue, 5, &elem);
      assert(r == 0);
      fprintf(stderr, "push: %d, size = %d\n", (int)(intptr_t) elem.__dtor, queue.__size);
      assert(queue.__size <= 59);
    }
  }
