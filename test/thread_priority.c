/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
    int current = _MCF_thread_get_priority(NULL);
    assert(current >= -15);
    assert(current <= +15);

    if(_MCF_thread_priority_idle <= current) {
      assert(_MCF_thread_set_priority(self, _MCF_thread_priority_idle) == 0);
      assert(_MCF_thread_get_priority(NULL) == _MCF_thread_priority_idle);
    }

    if(_MCF_thread_priority_low <= current) {
      assert(_MCF_thread_set_priority(self, _MCF_thread_priority_low) == 0);
      assert(_MCF_thread_get_priority(NULL) == _MCF_thread_priority_low);
    }

    if(_MCF_thread_priority_below_normal <= current) {
      assert(_MCF_thread_set_priority(self, _MCF_thread_priority_below_normal) == 0);
      assert(_MCF_thread_get_priority(NULL) == _MCF_thread_priority_below_normal);
    }

    if(_MCF_thread_priority_normal <= current) {
      assert(_MCF_thread_set_priority(self, _MCF_thread_priority_normal) == 0);
      assert(_MCF_thread_get_priority(NULL) == _MCF_thread_priority_normal);
    }

    if(_MCF_thread_priority_above_normal <= current) {
      assert(_MCF_thread_set_priority(self, _MCF_thread_priority_above_normal) == 0);
      assert(_MCF_thread_get_priority(NULL) == _MCF_thread_priority_above_normal);
    }

    if(_MCF_thread_priority_high <= current) {
      assert(_MCF_thread_set_priority(self, _MCF_thread_priority_high) == 0);
      assert(_MCF_thread_get_priority(NULL) == _MCF_thread_priority_high);
    }
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, NULL, 0);
    assert(thrd);

    printf("main waiting\n");
    _MCF_thread_wait(thrd, NULL);
    printf("main wait finished\n");

    _MCF_thread_wait(thrd, NULL);
  }
