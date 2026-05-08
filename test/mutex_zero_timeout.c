/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/mutex.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static _MCF_thread* threads[NTHREADS];
static _MCF_mutex mutex;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int resource = 0;

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&start, NULL);

    for(;;) {
      int r = _MCF_mutex_lock(&mutex, &(int64_t){ 0 });
      if(r == 0) {
        fprintf(stderr, "thread %d got %d\n", self->__tid, r);

        /* Add a resource.  */
        int old = resource;
        _MCF_sleep_noninterruptible(&(int64_t){ -10 });
        resource = old + 1;
        _MCF_mutex_unlock(&mutex);
        break;
      }
      else if(r == -1) {
        /* Wait.  */
        _MCF_sleep_noninterruptible(&(int64_t){ -10 });
        continue;
      }
      else
        assert(0);
    }

    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = _MCF_thread_new(thread_proc, NULL, 0);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(threads[k], NULL);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
