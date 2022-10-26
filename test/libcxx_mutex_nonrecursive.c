/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/libcxx.h"
#include "../src/clock.h"
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
