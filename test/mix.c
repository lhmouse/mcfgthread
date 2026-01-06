/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include "version.h"
#include <assert.h>
#include <windows.h>

int
main(void)
  {
    // load dlls
    wchar_t dll_name[1024];
    wsprintfW(dll_name, L".\\libmcfgthread-%d.dll", _MCF_ABI_VERSION_MAJOR);
    HMODULE pdll = LoadLibraryW(dll_name);
    assert(pdll);
    wsprintfW(dll_name, L".\\libmcfgthread-minimal-%d.dll", _MCF_ABI_VERSION_MAJOR);
    HMODULE mdll = LoadLibraryW(dll_name);
    assert(mdll);

    // load functions from dll
    typedef __typeof__(_MCF_tls_get) tls_get_fn;
    tls_get_fn* pdll_tls_get = __MCF_CAST_PTR(tls_get_fn, GetProcAddress(pdll, "_MCF_tls_get"));
    assert(pdll_tls_get);
    assert(pdll_tls_get != _MCF_tls_get);
    tls_get_fn* mdll_tls_get = __MCF_CAST_PTR(tls_get_fn, GetProcAddress(mdll, "_MCF_tls_get"));
    assert(mdll_tls_get);
    assert(mdll_tls_get != _MCF_tls_get);

    typedef __typeof__(_MCF_tls_xset) tls_xset_fn;
    tls_xset_fn* pdll_tls_xset = __MCF_CAST_PTR(tls_xset_fn, GetProcAddress(pdll, "_MCF_tls_xset"));
    assert(pdll_tls_xset);
    assert(pdll_tls_xset != _MCF_tls_xset);
    tls_xset_fn* mdll_tls_xset = __MCF_CAST_PTR(tls_xset_fn, GetProcAddress(mdll, "_MCF_tls_xset"));
    assert(mdll_tls_xset);
    assert(mdll_tls_xset != _MCF_tls_xset);

    // use common key
    _MCF_tls_key* key = _MCF_tls_key_new(__MCF_nullptr);
    assert(key);
    assert(_MCF_tls_get(key) == __MCF_nullptr);
    assert(pdll_tls_get(key) == __MCF_nullptr);
    assert(mdll_tls_get(key) == __MCF_nullptr);

    int dummy1 = 1;
    _MCF_tls_xset(key, __MCF_nullptr, &dummy1);
    assert(_MCF_tls_get(key) == &dummy1);
    assert(pdll_tls_get(key) == &dummy1);
    assert(mdll_tls_get(key) == &dummy1);

    int dummy2 = 2;
    pdll_tls_xset(key, __MCF_nullptr, &dummy2);
    assert(_MCF_tls_get(key) == &dummy2);
    assert(pdll_tls_get(key) == &dummy2);
    assert(mdll_tls_get(key) == &dummy2);
  }
