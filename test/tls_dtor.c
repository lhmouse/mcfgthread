/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include "../src/once.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_tls_key* key;
static _MCF_once start;
static int count;

static
void
tls_destructor(void* ptr)
  {
    printf("thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    __atomic_fetch_add((int*) ptr, 1, __ATOMIC_RELAXED);
  }

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_once_wait(&start, NULL);
    _MCF_once_wait(&start, NULL);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_once_release(&start);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], NULL);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(count == NTHREADS);
  }
