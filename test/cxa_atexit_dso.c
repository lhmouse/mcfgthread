/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxa.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static char buffer[1000];
static int dso_1, dso_2;

static
void __cdecl
cleanup_1(void* ptr)
  {
    strcat(buffer, ptr);
  }

static
void __thiscall
cleanup_2(void* ptr)
  {
    strcat(buffer, ptr);
  }

int
main(void)
  {
    int r;

    r = __MCF_cxa_atexit(cleanup_1, (void*) "a1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "b2", &dso_2);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "c1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "dN", NULL);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "e2", &dso_2);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "f1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "g1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "hN", NULL);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "i1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "j1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "k2", &dso_2);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_1, (void*) "lN", NULL);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "m1", &dso_1);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "n2", &dso_2);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "o2", &dso_2);
    assert(r == 0);
    r = __MCF_cxa_atexit(cleanup_2, (void*) "pN", NULL);
    assert(r == 0);

    assert(strcmp(buffer, "") == 0);
    __MCF_cxa_finalize(&dso_1);
    assert(strcmp(buffer, "m1j1i1g1f1c1a1") == 0);

    buffer[0] = 0;
    __MCF_cxa_finalize(&dso_2);
    assert(strcmp(buffer, "o2n2k2e2b2") == 0);

    buffer[0] = 0;
    __MCF_cxa_finalize(NULL);
    assert(strcmp(buffer, "pNlNhNdN") == 0);
  }
