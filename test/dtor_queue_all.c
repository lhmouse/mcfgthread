/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/dtor_queue.h"
#include <assert.h>
#include <stdio.h>

static __MCF_dtor_queue queue;

int
main(void)
  {
    __MCF_dtor_element elem = { 0 };
    int r;

    for(intptr_t k = 1;  k <= 1000;  ++k) {
      elem.__this = (void*) k;
      elem.__dso = (void*) (k % 10);

      r = __MCF_dtor_queue_push(&queue, &elem);
      assert(r == 0);
      printf("push: %td, size = %td\n", (intptr_t) elem.__this, (intptr_t) queue.__size);
    }

    for(intptr_t k = 1000;  k >= 1;  --k) {
      r = __MCF_dtor_queue_pop(&elem, &queue, __MCF_nullptr);
      assert(r == 0);
      printf("pop: %td, size = %td\n", (intptr_t) elem.__this, (intptr_t) queue.__size);

      assert(elem.__this == (void*) k);
      assert(elem.__dso == (void*) (k % 10));
    }

    r = __MCF_dtor_queue_pop(&elem, &queue, __MCF_nullptr);
    assert(r == -1);

    assert(queue.__size == 0);
    assert(queue.__prev == __MCF_nullptr);
  }
