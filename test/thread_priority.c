/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
    int current = _MCF_thread_get_priority(__MCF_nullptr);
    assert(current >= _MCF_thread_priority_idle);
    assert(current <= _MCF_thread_priority_realtime);

    assert(_MCF_thread_set_priority(self, _MCF_thread_priority_idle) == 0);
    assert(_MCF_thread_get_priority(__MCF_nullptr) == _MCF_thread_priority_idle);

    assert(_MCF_thread_set_priority(self, _MCF_thread_priority_low) == 0);
    assert(_MCF_thread_get_priority(__MCF_nullptr) == _MCF_thread_priority_low);

    assert(_MCF_thread_set_priority(self, _MCF_thread_priority_below_normal) == 0);
    assert(_MCF_thread_get_priority(__MCF_nullptr) == _MCF_thread_priority_below_normal);

    assert(_MCF_thread_set_priority(self, _MCF_thread_priority_normal) == 0);
    assert(_MCF_thread_get_priority(__MCF_nullptr) == _MCF_thread_priority_normal);

    assert(_MCF_thread_set_priority(self, _MCF_thread_priority_above_normal) == 0);
    assert(_MCF_thread_get_priority(__MCF_nullptr) == _MCF_thread_priority_above_normal);

    assert(_MCF_thread_set_priority(self, _MCF_thread_priority_high) == 0);
    assert(_MCF_thread_get_priority(__MCF_nullptr) == _MCF_thread_priority_high);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, __MCF_nullptr, 0);
    assert(thrd);

    printf("main waiting\n");
    _MCF_thread_wait(thrd, __MCF_nullptr);
    printf("main wait finished\n");

    _MCF_thread_wait(thrd, __MCF_nullptr);
  }
