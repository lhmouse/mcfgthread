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
    _MCF_thread* thrd = _MCF_thread_new_aligned(thread_proc, 3, NULL, 0);
    assert(thrd == NULL);
    assert(GetLastError() == ERROR_NOT_SUPPORTED);

    thrd = _MCF_thread_new_aligned(thread_proc, 0x40000000, NULL, 0);
    assert(thrd == NULL);
    assert(GetLastError() == ERROR_NOT_SUPPORTED);
  }
