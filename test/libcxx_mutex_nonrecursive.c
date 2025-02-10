/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static __libcpp_mutex_t mutex = _LIBCPP_MUTEX_INITIALIZER;

int
main(void)
  {
    int r;

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == true);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == false);

    r = __libcpp_mutex_unlock(&mutex);
    assert(r == 0);

    r = __libcpp_mutex_trylock(&mutex);
    assert(r == true);
  }
