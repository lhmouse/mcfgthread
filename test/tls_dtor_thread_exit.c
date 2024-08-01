/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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

    static int value = 0;
    int r = _MCF_tls_set(key, &value);
    assert(r == 0);

    _MCF_thread_exit();
    __MCF__Exit(42);
  }
