// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/dtorque.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static __MCF_dtorque queue;

int
main(void)
  {
    __MCF_dtorelem elem = { 0 };
    int r;

    for(intptr_t k = 1;  k <= 1000;  ++k) {
      elem.__this = (void*) k;
      elem.__dso = (void*) (k % 10);

      r = __MCF_dtorque_push(&queue, &elem);
      assert(r == 0);
      printf("push: %td, size = %td\n", (intptr_t) elem.__this, (intptr_t) queue.__size);

    }

    for(intptr_t k = 99;  k >= 0;  --k) {
      r = __MCF_dtorque_pop(&elem, &queue, (void*) 1);
      assert(r == 0);
      printf("pop: %td, size = %td\n", (intptr_t) elem.__this, (intptr_t) queue.__size);

      assert(elem.__this == (void*) (k * 10 + 1));
      assert(elem.__dso == (void*) 1);
    }

    r = __MCF_dtorque_pop(&elem, &queue, (void*) 1);
    assert(r == -1);

    r = __MCF_dtorque_pop(&elem, &queue, (void*) 2);
    assert(r == 0);
  }
