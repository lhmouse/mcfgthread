/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxa.h"
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
    static int value = 0;
    __MCF_cxa_thread_atexit(tls_dtor, &value, NULL);

    __MCF_exit(42);
  }
