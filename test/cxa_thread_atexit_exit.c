/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/exit.h"
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
