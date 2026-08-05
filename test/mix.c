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
    // load dlls
    static const char* const s_dll_prefixes[] =
      {
#if defined __CYGWIN__
        "cyg",
        "msys-",
#else
        "lib",
        "",
#endif
        NULL
      };

    HMODULE pdll = NULL;
    HMODULE mdll = NULL;

    for(const char* const* ppfx = s_dll_prefixes;  *ppfx && !pdll && !mdll;  ppfx ++) {
      wchar_t path[128];
      wsprintfW(path, L"./%hsmcfgthread-%d.dll", *ppfx, _MCF_ABI_VERSION_MAJOR);
      pdll = LoadLibraryW(path);
      wsprintfW(path, L"./%hsmcfgthread-minimal-%d.dll", *ppfx, _MCF_ABI_VERSION_MAJOR);
      mdll = LoadLibraryW(path);
    }

    assert(pdll);
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
    _MCF_tls_key* key = _MCF_tls_key_new(NULL);
    assert(key);
    assert(_MCF_tls_get(key) == NULL);
    assert(pdll_tls_get(key) == NULL);
    assert(mdll_tls_get(key) == NULL);

    int dummy1 = 1;
    _MCF_tls_xset(key, NULL, &dummy1);
    assert(_MCF_tls_get(key) == &dummy1);
    assert(pdll_tls_get(key) == &dummy1);
    assert(mdll_tls_get(key) == &dummy1);

    int dummy2 = 2;
    pdll_tls_xset(key, NULL, &dummy2);
    assert(_MCF_tls_get(key) == &dummy2);
    assert(pdll_tls_get(key) == &dummy2);
    assert(mdll_tls_get(key) == &dummy2);
  }
