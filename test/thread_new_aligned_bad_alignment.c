/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/thread.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new_aligned(thread_proc, 3, NULL, 0);
    assert(thrd == NULL);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    thrd = _MCF_thread_new_aligned(thread_proc, 0x40000000, NULL, 0);
    assert(thrd == NULL);
    assert(GetLastError() == ERROR_NOT_SUPPORTED);
  }
