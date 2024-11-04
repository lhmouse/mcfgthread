/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

HANDLE thrd;

static
void
tls_dtor(void* ptr)
  {
    __MCF__Exit(*(int*) ptr);
  }

static
DWORD
__stdcall
thread_proc(LPVOID param)
  {
    _MCF_tls_key* key = _MCF_tls_key_new(tls_dtor);
    assert(key);

    static int value = 0;
    int r = _MCF_tls_set(key, &value);
    assert(r == 0);

    (void) param;
    _MCF_thread_exit();
    __MCF__Exit(42);
  }

int
main(void)
  {
    thrd = CreateThread(__MCF_nullptr, 0, thread_proc, __MCF_nullptr, 0, __MCF_nullptr);
    assert(thrd);

    printf("main waiting\n");
    WaitForSingleObject(thrd, INFINITE);
    printf("main wait finished\n");

    __MCF__Exit(43);
  }
