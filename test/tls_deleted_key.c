/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include "../src/once.h"
#include <assert.h>
#include <stdio.h>

static _MCF_tls_key* key;
static _MCF_thread* thrd;
static _MCF_once thread_start;
static _MCF_once value_set;
static _MCF_once key_deleted;
static int count;

static
void
tls_destructor(void* ptr)
  {
    (void) ptr;
    printf("thread %d tls_destructor\n", (int) _MCF_thread_self_tid());
    __atomic_fetch_add(&count, 1, __ATOMIC_RELAXED);
  }

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_once_wait(&thread_start, NULL);
    _MCF_once_wait(&thread_start, NULL);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);
    printf("thread %d set value\n", self->__tid);

    _MCF_once_release(&value_set);
    _MCF_once_wait(&key_deleted, NULL);
    _MCF_once_wait(&key_deleted, NULL);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);

    thrd = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thrd);

    printf("main waiting for value_set\n");
    _MCF_once_release(&thread_start);
    _MCF_once_wait(&value_set, NULL);
    _MCF_once_wait(&value_set, NULL);

    _MCF_tls_key_delete(key);
    key = NULL;
    printf("main deleted key; waiting for termination\n");
    _MCF_once_release(&key_deleted);

    _MCF_thread_wait(thrd, NULL);
    printf("main wait finished\n");

    assert(count == 0);
  }
