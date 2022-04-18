/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static __gthread_key_t key;
static __gthread_t thrd;
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

static void*
thread_proc(void* param)
  {
    (void) param;
    WaitForSingleObject(thread_start, INFINITE);

    int r = __gthread_setspecific(key, &count);
    assert(r == 0);
    printf("thread %d set value\n", (int) GetCurrentThreadId());

    SetEvent(value_set);
    WaitForSingleObject(key_deleted, INFINITE);

    printf("thread %d quitting\n", (int) GetCurrentThreadId());
    return NULL;
  }

int
main(void)
  {
    int r = __gthread_key_create(&key, tls_destructor);
    assert(r == 0);
    assert(key);

    thread_start = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(thread_start);
    value_set = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(value_set);
    key_deleted = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(key_deleted);

    r = __gthread_create(&thrd, thread_proc, NULL);
    assert(r == 0);
    assert(thrd);

    printf("main waiting for value_set\n");
    SetEvent(thread_start);
    WaitForSingleObject(value_set, INFINITE);

    __gthread_key_delete(key);
    key = NULL;
    printf("main deleted key; waiting for termination\n");
    SetEvent(key_deleted);

    __gthread_join(thrd, NULL);
    printf("main wait finished\n");

    assert(count == 0);
  }
