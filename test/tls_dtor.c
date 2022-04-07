// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_tls_key* key;
static HANDLE event;
static int count;

static void
tls_destructor(void* ptr)
  {
    printf("thread %d tls_destructor\n", (int) GetCurrentThreadId());
    __atomic_fetch_add((int*) ptr, 1, __ATOMIC_RELAXED);
  }

static void
thread_proc(_MCF_thread* self)
  {
    WaitForSingleObject(event, INFINITE);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);

    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(event);

#define NTHREADS  64
    _MCF_thread* threads[NTHREADS];
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    SetEvent(event);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      WaitForSingleObject(threads[k]->__handle, INFINITE);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
