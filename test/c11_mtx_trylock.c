/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];
static mtx_t mutex;
static _MCF_sem start = __MCF_SEM_INIT(0);
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
        fprintf(stderr, "thread %d got %d\n", __MCF_tid(), r);

        r = mtx_lock(&mutex);
        assert(r == 0);
        r = mtx_trylock(&mutex);
        assert(r == 0);

        /* Add a resource.  */
        int old = resource;
        _MCF_sleep_noninterruptible(&(int64_t){ -10 });
        resource = old + 1;
        mtx_unlock(&mutex);
        mtx_unlock(&mutex);
        mtx_unlock(&mutex);
        break;
      }
      else if(r == thrd_busy) {
        /* Wait.  */
        _MCF_sleep_noninterruptible(&(int64_t){ -10 });
        continue;
      }
      else
        assert(0);
    }

    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    return 0;
  }

int
main(void)
  {
    int err = mtx_init(&mutex, mtx_recursive);
    assert(err == thrd_success);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, NULL);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], NULL);
      assert(r == thrd_success);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
