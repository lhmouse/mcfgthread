/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  64U
static HANDLE threads[NTHREADS];
static _MCF_tls_key* key;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int count;

static
void
tls_destructor(void* ptr)
  {
    fprintf(stderr, "thread %d tls_destructor\n", __MCF_tid());
    _MCF_atomic_xadd_32_rlx(ptr, 1);
  }

static
DWORD
__stdcall
thread_proc(LPVOID param)
  {
    _MCF_sem_wait(&start, NULL);

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
      threads[k] = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);
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
