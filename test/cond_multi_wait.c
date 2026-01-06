/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cond.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static _MCF_thread* thread1;
static _MCF_thread* thread2;
static _MCF_cond cond;
static int32_t signal_count;

static
void
thread_proc(_MCF_thread* self)
  {
    (void) self;

    const int64_t start_time = _MCF_tick_count();
    while(_MCF_tick_count() < start_time + 3000) {
      _MCF_cond_wait(&cond, __MCF_nullptr, __MCF_nullptr, 0, (const int64_t[]){ 0 });

      // Check whether a deadlock has occurred.
      //   https://github.com/lhmouse/mcfgthread/issues/86
      int32_t c1 = _MCF_atomic_load_32_rlx(&signal_count);
      if(signal_count & 1) {
        // Main thread is waiting.
        _MCF_sleep((const int64_t[]){ -10 });
        int32_t c2 = _MCF_atomic_load_32_rlx(&signal_count);
        if(c1 == c2) {
          fprintf(stderr, "maybe deadlock\n");
          break;
        }
      }
    }
  }

int
main(void)
  {
    thread1 = _MCF_thread_new(thread_proc, __MCF_nullptr, 0);
    assert(thread1);
    thread2 = _MCF_thread_new(thread_proc, __MCF_nullptr, 0);
    assert(thread2);

    const int64_t start_time = _MCF_tick_count();
    while(_MCF_tick_count() < start_time + 3000) {
      _MCF_atomic_xadd_32_rlx(&signal_count, 1);
      _MCF_cond_signal_all(&cond);
      _MCF_atomic_xadd_32_rlx(&signal_count, 1);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(thread1, __MCF_nullptr);
    _MCF_thread_wait(thread2, __MCF_nullptr);
  }
