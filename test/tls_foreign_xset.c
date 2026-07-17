/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#undef NDEBUG
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
    assert(p == NULL);

    p = &r;
    r = _MCF_tls_xset(key, &p, &dso_2);
    assert(r == 0);
    assert(p == NULL);

    p = _MCF_tls_get(key);
    assert(p == &dso_2);

    p = &r;
    r = _MCF_tls_xset(key, &p, NULL);
    assert(r == 0);
    assert(p == &dso_2);

    _MCF_sleep_noninterruptible(&(int64_t){ -1001 });
    fprintf(stderr, "thread %d quitting\n", (int) GetCurrentThreadId());
    (void) param;
    return 0;
  }

int
main(void)
  {
    void* p;
    int r;

    key = _MCF_tls_key_new(NULL);
    assert(key);
    assert(_MCF_tls_key_get_destructor(key) == NULL);

    p = _MCF_tls_get(key);
    assert(p == NULL);

    p = &r;
    r = _MCF_tls_xset(key, &p, NULL);
    assert(r == 0);
    assert(p == NULL);

    p = &r;
    r = _MCF_tls_xset(key, &p, &dso_1);
    assert(r == 0);
    assert(p == NULL);

    p = _MCF_tls_get(key);
    assert(p == &dso_1);

    p = &r;
    r = _MCF_tls_xset(key, &p, &dso_2);
    assert(r == 0);
    assert(p == &dso_1);

    thrd = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    WaitForSingleObject(thrd, INFINITE);
    fprintf(stderr, "main wait finished\n");

    p = _MCF_tls_get(key);
    assert(p == &dso_2);

    p = &r;
    r = _MCF_tls_xset(key, &p, NULL);
    assert(r == 0);
    assert(p == &dso_2);
  }
