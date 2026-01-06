/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static _MCF_tls_key* key;
static _MCF_thread* thrd;
static int dso_1, dso_2;

static
void
thread_proc(_MCF_thread* self)
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

    _MCF_sleep((const int64_t[]) { -1001 });
    fprintf(stderr, "thread %d quitting\n", self->__tid);
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

    thrd = _MCF_thread_new(thread_proc, __MCF_nullptr, sizeof(int));
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(thrd, __MCF_nullptr);
    fprintf(stderr, "main wait finished\n");

    p = _MCF_tls_get(key);
    assert(p == &dso_2);

    p = &r;
    r = _MCF_tls_xset(key, &p, __MCF_nullptr);
    assert(r == 0);
    assert(p == &dso_2);
  }
