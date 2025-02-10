/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <assert.h>
#include <windows.h>

int
main(void)
  {
    _MCF_tls_key* key = _MCF_tls_key_new(__MCF_nullptr);
    assert(key);

    void* p;
    int r;

    p = _MCF_tls_get(key);
    assert(p == __MCF_nullptr);

    r = _MCF_tls_set(key, &r);
    assert(r == 0);

    p = _MCF_tls_get(key);
    assert(p == &r);

    _MCF_tls_key_add_ref(key);
    _MCF_tls_key_delete(key);

    p = _MCF_tls_get(key);
    assert(p == __MCF_nullptr);

    r = _MCF_tls_set(key, &r);
    assert(r == -1);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    p = _MCF_tls_get(key);
    assert(p == __MCF_nullptr);
  }
