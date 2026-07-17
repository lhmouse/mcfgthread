/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static
void
thread_atexit_proc(void* arg)
  {
    fprintf(stderr, "thread %d atexit\n", __MCF_tid());
    *(int*) arg = 42;
  }

static
void
thread_proc(_MCF_thread* self)
  {
    __MCF_cxa_thread_atexit(thread_atexit_proc, _MCF_thread_get_data(self), NULL);
    _MCF_sleep_noninterruptible(&(int64_t){ -1001 });
    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    _MCF_thread* thrd = _MCF_thread_new(thread_proc, NULL, sizeof(int));
    assert(thrd);

    int* value = _MCF_thread_get_data(thrd);
    assert(*value == 0);

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(thrd, NULL);
    fprintf(stderr, "main wait finished\n");

    assert(*value == 42);
  }
