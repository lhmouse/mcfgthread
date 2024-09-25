/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include "version.h"
#include <assert.h>
#include <windows.h>
#include <wchar.h>

int
main(void)
  {
#if defined __CYGWIN__
    return 77;
#else
    // load functions from dll
    wchar_t dll_name[100];
    swprintf(dll_name, 100, L"libmcfgthread-%d.dll", _MCF_ABI_VERSION_MAJOR);
    HMODULE dll = LoadLibraryW(dll_name);
    assert(dll);

    typedef __typeof__(_MCF_tls_get) pdll_tls_get_fn;
    pdll_tls_get_fn* pdll_tls_get = (pdll_tls_get_fn*)(INT_PTR) GetProcAddress(dll, "_MCF_tls_get");
    assert(pdll_tls_get);
    assert(pdll_tls_get != _MCF_tls_get);

    typedef __typeof__(_MCF_tls_xset) pdll_tls_xset_fn;
    pdll_tls_xset_fn* pdll_tls_xset = (pdll_tls_xset_fn*)(INT_PTR) GetProcAddress(dll, "_MCF_tls_xset");
    assert(pdll_tls_xset);
    assert(pdll_tls_xset != _MCF_tls_xset);

    // use common key
    _MCF_tls_key* key = _MCF_tls_key_new(__MCF_nullptr);
    assert(key);
    assert(_MCF_tls_get(key) == __MCF_nullptr);
    assert(pdll_tls_get(key) == __MCF_nullptr);

    int dummy1;
    _MCF_tls_xset(key, __MCF_nullptr, &dummy1);
    assert(_MCF_tls_get(key) == &dummy1);
    assert(pdll_tls_get(key) == &dummy1);

    int dummy2;
    pdll_tls_xset(key, __MCF_nullptr, &dummy2);
    assert(_MCF_tls_get(key) == &dummy2);
    assert(pdll_tls_get(key) == &dummy2);
#endif
  }
