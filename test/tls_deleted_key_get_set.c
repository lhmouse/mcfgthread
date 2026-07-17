/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

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
