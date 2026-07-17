/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/exit.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static
void
tls_dtor(void* ptr)
  {
    __MCF__Exit(*(int*) ptr);
  }

int
main(void)
  {
    _MCF_tls_key* key = _MCF_tls_key_new(tls_dtor);
    assert(key);

    static int value = 0;
    int r = _MCF_tls_set(key, &value);
    assert(r == 0);

    _MCF_thread_exit();
    __MCF__Exit(42);
  }
