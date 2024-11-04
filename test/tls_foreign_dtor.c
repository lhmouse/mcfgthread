/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  64U
static HANDLE threads[NTHREADS];
static _MCF_tls_key* key;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int count;

static
void
tls_destructor(void* ptr)
  {
    fprintf(stderr, "thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    _MCF_atomic_xadd_32_rlx(ptr, 1);
  }

static __attribute__((__stdcall__))
DWORD
thread_proc(LPVOID param)
  {
    _MCF_sem_wait(&start, __MCF_nullptr);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    fprintf(stderr, "thread %d quitting\n", (int) GetCurrentThreadId());
    (void) param;
    return 0;
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);
    assert(_MCF_tls_key_get_destructor(key) == tls_destructor);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = CreateThread(__MCF_nullptr, 0, thread_proc, __MCF_nullptr, 0, __MCF_nullptr);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      WaitForSingleObject(threads[k], INFINITE);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
