/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/thread.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    assert(_MCF_thread_new(thread_proc, __MCF_nullptr, PTRDIFF_MAX) == __MCF_nullptr);
    assert(GetLastError() == ERROR_ARITHMETIC_OVERFLOW);
  }
