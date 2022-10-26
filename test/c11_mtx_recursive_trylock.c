/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
#include "../src/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];
static mtx_t mutex;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, NULL);

    for(;;) {
      int r = mtx_trylock(&mutex);
      if(r == thrd_success) {
        printf("thread %d got %d\n", (int) _MCF_thread_self_tid(), r);

        /* Add a resource.  */
        int old = resource;
        _MCF_sleep((const int64_t[]) { -10 });
        resource = old + 1;
        mtx_unlock(&mutex);
        break;
      }
      else if(r == thrd_busy) {
        /* Wait.  */
        _MCF_sleep((const int64_t[]) { -10 });
        continue;
      }
      else
        assert(0);
    }

    printf("thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    int err = mtx_init(&mutex, mtx_plain);
    assert(err == thrd_success);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, NULL);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    printf("main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], NULL);
      assert(r == thrd_success);
      printf("main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
