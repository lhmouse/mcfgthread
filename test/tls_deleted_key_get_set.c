/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

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

    p = _MCF_tls_get(key);
    assert(p == __MCF_nullptr);
  }
