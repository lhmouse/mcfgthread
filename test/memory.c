/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/xglobals.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <winbase.h>
#include <ntsecapi.h>

static char comp[100];
static char data[100];

int
main(void)
  {
    HMODULE msvcrt = LoadLibraryW(L"msvcrt.dll");
    assert(msvcrt);

    typedef void __cdecl memmove_t(void*, const void*, size_t);
    memmove_t* pmemmove = (memmove_t*)(intptr_t) GetProcAddress(msvcrt, "memmove");
    assert(pmemmove);

    typedef void __cdecl memset_t(void*, int, size_t);
    memset_t* pmemset = (memset_t*)(intptr_t) GetProcAddress(msvcrt, "memset");
    assert(pmemset);

    typedef int __cdecl memcmp_t(const void*, const void*, size_t);
    memcmp_t* pmemcmp = (memcmp_t*)(intptr_t) GetProcAddress(msvcrt, "memcmp");
    assert(pmemcmp);

    // __MCF_mfill
    RtlGenRandom(comp, sizeof(comp));
    pmemmove(data, comp, sizeof(comp));

    pmemset(comp + 20, 'b', 30);
    char* ptr = __MCF_mfill(data + 20, 'b', 30);
    assert(ptr == data + 20);

    pmemset(comp + 50, 'c', 20);
    ptr = __MCF_mfill(data + 50, 'c', 20);
    assert(ptr == data + 50);

    assert(pmemcmp(comp, data, sizeof(comp)) == 0);
    assert(memcmp(comp, data, sizeof(comp)) == 0);

    // __MCF_mzero
    RtlGenRandom(comp, sizeof(comp));
    pmemmove(data, comp, sizeof(comp));

    pmemset(comp + 20, 0, 30);
    ptr = __MCF_mzero(data + 20, 30);
    assert(ptr == data + 20);

    pmemset(comp + 50, 0, 20);
    ptr = __MCF_mzero(data + 50, 20);
    assert(ptr == data + 50);

    assert(pmemcmp(comp, data, sizeof(comp)) == 0);
    assert(memcmp(comp, data, sizeof(comp)) == 0);

    // __MCF_mcopy
    RtlGenRandom(comp, sizeof(comp));
    pmemmove(data, comp, sizeof(comp));

    pmemmove(comp + 10, comp + 20, 40);
    ptr = __MCF_mcopy(data + 10, data + 20, 40);
    assert(ptr == data + 10);

    assert(pmemcmp(comp, data, sizeof(comp)) == 0);
    assert(memcmp(comp, data, sizeof(comp)) == 0);

    // __MCF_mcopy_backward
    RtlGenRandom(comp, sizeof(comp));
    pmemmove(data, comp, sizeof(comp));

    pmemmove(comp + 20, comp + 10, 40);
    ptr = __MCF_mcopy_backward(data + 20, data + 10, 40);
    assert(ptr == data + 20);

    assert(pmemcmp(comp, data, sizeof(comp)) == 0);
    assert(memcmp(comp, data, sizeof(comp)) == 0);

    // __MCF_mcomp (equal)
    RtlGenRandom(comp, sizeof(comp));
    pmemmove(data, comp, sizeof(comp));

    assert(pmemcmp(comp, data, sizeof(comp)) == 0);
    assert(memcmp(comp, data, sizeof(comp)) == 0);

    // __MCF_mcomp (less 2)
    comp[72] = '1';
    data[72] = '2';

    assert(pmemcmp(comp, data, sizeof(comp)) < 0);
    assert(memcmp(comp, data, sizeof(comp)) < 0);

    // __MCF_mcomp (greater 1)
    comp[71] = '\x80';
    data[71] = '\x7F';

    assert(pmemcmp(comp, data, sizeof(comp)) > 0);
    assert(memcmp(comp, data, sizeof(comp)) > 0);

    // __MCF_mcomp (greater 2)
    comp[45] = '2';
    data[45] = '1';

    assert(pmemcmp(comp, data, sizeof(comp)) > 0);
    assert(memcmp(comp, data, sizeof(comp)) > 0);

    // __MCF_mcomp (less 1)
    comp[44] = '\x7F';
    data[44] = '\x80';

    assert(pmemcmp(comp, data, sizeof(comp)) < 0);
    assert(memcmp(comp, data, sizeof(comp)) < 0);
  }
