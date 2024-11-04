/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_tls_key* key;
static HANDLE thrd;
static int dso_1, dso_2;

static
DWORD
__stdcall
thread_proc(LPVOID param)
  {
    void* p;
    int r;

    p = _MCF_tls_get(key);
    assert(p == __MCF_nullptr);

    p = &r;
    r = _MCF_tls_xset(key, &p, &dso_2);
    assert(r == 0);
    assert(p == __MCF_nullptr);

    p = _MCF_tls_get(key);
    assert(p == &dso_2);

    p = &r;
    r = _MCF_tls_xset(key, &p, __MCF_nullptr);
    assert(r == 0);
    assert(p == &dso_2);

    _MCF_sleep((const int64_t[]) { -1000 });
    printf("thread %d quitting\n", (int) GetCurrentThreadId());
    (void) param;
    return 0;
  }

int
main(void)
  {
    void* p;
    int r;

    key = _MCF_tls_key_new(__MCF_nullptr);
    assert(key);
    assert(_MCF_tls_key_get_destructor(key) == __MCF_nullptr);

    p = _MCF_tls_get(key);
    assert(p == __MCF_nullptr);

    p = &r;
    r = _MCF_tls_xset(key, &p, __MCF_nullptr);
    assert(r == 0);
    assert(p == __MCF_nullptr);

    p = &r;
    r = _MCF_tls_xset(key, &p, &dso_1);
    assert(r == 0);
    assert(p == __MCF_nullptr);

    p = _MCF_tls_get(key);
    assert(p == &dso_1);

    p = &r;
    r = _MCF_tls_xset(key, &p, &dso_2);
    assert(r == 0);
    assert(p == &dso_1);

    thrd = CreateThread(__MCF_nullptr, 0, thread_proc, __MCF_nullptr, 0, __MCF_nullptr);
    assert(thrd);

    printf("main waiting\n");
    WaitForSingleObject(thrd, INFINITE);
    printf("main wait finished\n");

    p = _MCF_tls_get(key);
    assert(p == &dso_2);

    p = &r;
    r = _MCF_tls_xset(key, &p, __MCF_nullptr);
    assert(r == 0);
    assert(p == &dso_2);
  }
