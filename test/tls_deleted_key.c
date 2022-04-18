/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_tls_key* key;
static _MCF_thread* thrd;
static HANDLE thread_start;
static HANDLE value_set;
static HANDLE key_deleted;
static int count;

static void
tls_destructor(void* ptr)
  {
    (void) ptr;
    printf("thread %d tls_destructor\n", (int) GetCurrentThreadId());
    __atomic_fetch_add(&count, 1, __ATOMIC_RELAXED);
  }

static void
thread_proc(_MCF_thread* self)
  {
    WaitForSingleObject(thread_start, INFINITE);

    int r = _MCF_tls_set(key, &count);
    assert(r == 0);
    printf("thread %d set value\n", self->__tid);

    SetEvent(value_set);
    WaitForSingleObject(key_deleted, INFINITE);

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    key = _MCF_tls_key_new(tls_destructor);
    assert(key);

    thread_start = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(thread_start);
    value_set = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(value_set);
    key_deleted = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(key_deleted);

    thrd = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thrd);

    printf("main waiting for value_set\n");
    SetEvent(thread_start);
    WaitForSingleObject(value_set, INFINITE);

    _MCF_tls_key_delete(key);
    key = NULL;
    printf("main deleted key; waiting for termination\n");
    SetEvent(key_deleted);

    WaitForSingleObject(thrd->__handle, INFINITE);
    printf("main wait finished\n");

    assert(count == 0);
  }
