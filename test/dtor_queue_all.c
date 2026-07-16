/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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

    for(intptr_t k = 1;  k <= 1000;  ++k) {
      elem.__dtor = (__MCF_cxa_dtor_cdecl*) k;
      elem.__dso = (void*) (k % 10);

      r = __MCF_dtor_queue_push(&queue, &elem);
      assert(r == 0);
      fprintf(stderr, "push: %td, size = %d\n", (intptr_t) elem.__dtor, queue.__size);
    }

    for(intptr_t k = 1000;  k >= 1;  --k) {
      r = __MCF_dtor_queue_pop(&elem, &queue, NULL);
      assert(r == 0);
      fprintf(stderr, "pop: %td, size = %d\n", (intptr_t) elem.__dtor, queue.__size);

      assert(elem.__dtor == (__MCF_cxa_dtor_cdecl*) k);
      assert(elem.__dso == (void*) (k % 10));
    }

    r = __MCF_dtor_queue_pop(&elem, &queue, NULL);
    assert(r == -1);

    assert(queue.__size == 0);
    assert(queue.__prev == NULL);
  }
