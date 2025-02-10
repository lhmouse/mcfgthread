/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static
void
thread_atexit_proc(void* arg)
  {
    fprintf(stderr, "thread %d atexit\n", (int) _MCF_thread_self_tid());
    *(int*) arg = 42;
  }

static
void
thread_proc(_MCF_thread* self)
  {
    __MCF_cxa_thread_atexit(thread_atexit_proc, _MCF_thread_get_data(self), __MCF_nullptr);
    _MCF_sleep((const int64_t[]) { -1001 });
    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, __MCF_nullptr, sizeof(int));
    assert(thrd);

    int* value = _MCF_thread_get_data(thrd);
    assert(*value == 0);

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(thrd, __MCF_nullptr);
    fprintf(stderr, "main wait finished\n");

    assert(*value == 42);
  }
