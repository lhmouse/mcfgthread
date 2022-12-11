/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/thread.h"
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
    _MCF_tls_key* key = _MCF_tls_key_new(tls_dtor);
    assert(key);

    static int value = 42;
    int r = _MCF_tls_set(key, &value);
    assert(r == 0);

    __MCF__Exit(0);
  }
