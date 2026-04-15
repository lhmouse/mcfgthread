/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#define __MCF_EXPAND_INLINE_DEFINITIONS 1
#include "../mcfgthread/shared_mutex.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define NTHREADS  16U
static _MCF_thread* reader_threads[NTHREADS];
static _MCF_thread* writer_threads[NTHREADS];
static _MCF_shared_mutex mutex;
static _MCF_sem start = __MCF_SEM_INIT(0);

#define NITER  10000U
static int res1 = 0, res2 = 0;

static
void
reader_thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&start, NULL);

    for(uint32_t i = 0; i < NITER;  ++i) {
      int r = _MCF_shared_mutex_lock_shared(&mutex, NULL);
      assert(r == 0);

      /* Check resources are equal.  */
      int t1 = res1;
      SwitchToThread();
      assert(t1 == res2);
      _MCF_shared_mutex_unlock(&mutex);

      //fprintf(stderr, "R    thread %d\n", __MCF_tid());
      SwitchToThread();
    }

    fprintf(stderr, "R    thread %d quitting\n", self->__tid);
  }

static
void
writer_thread_proc(_MCF_thread* self)
  {
    _MCF_sem_wait(&start, NULL);

    for(uint32_t i = 0; i < NITER;  ++i) {
      int r = _MCF_shared_mutex_lock_exclusive(&mutex, NULL);
      assert(r == 0);

      /* Add resources.  */
      res1 ++;
      SwitchToThread();
      res2 ++;
      assert(res1 == res2);
      _MCF_shared_mutex_unlock(&mutex);

      //fprintf(stderr, "  W  thread %d\n", __MCF_tid());
      SwitchToThread();
    }

    fprintf(stderr, "  W  thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      reader_threads[k] = _MCF_thread_new(reader_thread_proc, NULL, 0);
      assert(reader_threads[k]);
      writer_threads[k] = _MCF_thread_new(writer_thread_proc, NULL, 0);
      assert(writer_threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS * 2);

    for(size_t k = 0;  k < NTHREADS;  ++k) {
      _MCF_thread_wait(reader_threads[k], NULL);
      _MCF_thread_wait(writer_threads[k], NULL);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(res1 == NITER * NTHREADS);
    assert(res2 == NITER * NTHREADS);
  }
