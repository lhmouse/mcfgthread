/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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
      fprintf(stderr, "push: %d, size = %d\n", (int)(intptr_t) elem.__this, (int)(intptr_t) queue.__size);
    }

    for(intptr_t k = 1000;  k >= 1;  --k) {
      r = __MCF_dtor_queue_pop(&elem, &queue, __MCF_nullptr);
      assert(r == 0);
      fprintf(stderr, "pop: %d, size = %d\n", (int)(intptr_t) elem.__this, (int)(intptr_t) queue.__size);

      assert(elem.__this == (void*) k);
      assert(elem.__dso == (void*) (k % 10));
    }

    r = __MCF_dtor_queue_pop(&elem, &queue, __MCF_nullptr);
    assert(r == -1);

    assert(queue.__size == 0);
    assert(queue.__prev == __MCF_nullptr);
  }
