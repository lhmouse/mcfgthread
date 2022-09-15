/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
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
    assert(_MCF_thread_new(thread_proc, NULL, PTRDIFF_MAX) == NULL);
    assert(GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
  }
