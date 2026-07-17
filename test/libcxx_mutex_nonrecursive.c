/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
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
