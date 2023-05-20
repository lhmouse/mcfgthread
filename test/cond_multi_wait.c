/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

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
      _MCF_cond_wait(&cond, NULL, NULL, 0, (const int64_t[]){ 0 });

      // Check whether a deadlock has occurred.
      //   https://github.com/lhmouse/mcfgthread/issues/86
      int32_t c1 = _MCF_atomic_load_32_rlx(&signal_count);
      if(signal_count & 1) {
        // Main thread is waiting.
        _MCF_sleep((const int64_t[]){ -10 });
        int32_t c2 = _MCF_atomic_load_32_rlx(&signal_count);
        if(c1 == c2) {
          printf("maybe deadlock\n");
          break;
        }
      }
    }
  }

int
main(void)
  {
    thread1 = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thread1);
    thread2 = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thread2);

    const int64_t start_time = _MCF_tick_count();
    while(_MCF_tick_count() < start_time + 3000) {
      _MCF_atomic_xadd_32_rlx(&signal_count, 1);
      _MCF_cond_signal_all(&cond);
      _MCF_atomic_xadd_32_rlx(&signal_count, 1);
    }

    printf("main waiting\n");
    _MCF_thread_wait(thread1, NULL);
    _MCF_thread_wait(thread2, NULL);
  }
