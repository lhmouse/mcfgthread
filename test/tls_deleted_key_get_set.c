/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <windows.h>

int
main(void)
  {
    _MCF_tls_key* key = _MCF_tls_key_new(NULL);
    assert(key);

    void* p;
    int r;
    int dummy1 = 1;

    p = _MCF_tls_get(key);
    assert(p == NULL);

    r = _MCF_tls_set(key, &dummy1);
    assert(r == 0);

    p = _MCF_tls_get(key);
    assert(p == &dummy1);

    _MCF_tls_key_add_ref(key);
    _MCF_tls_key_delete(key);

    p = _MCF_tls_get(key);
    assert(p == NULL);

    r = _MCF_tls_set(key, &dummy1);
    assert(r == -1);

    p = _MCF_tls_get(key);
    assert(p == NULL);
  }
