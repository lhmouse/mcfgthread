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
    void* data = _MCF_thread_get_data(self);
    assert((uintptr_t) data % __MCF_THREAD_DATA_ALIGNMENT == 0);
    assert(RtlCompareMemory(data, "hello", 5) == 5);  // all equal

    printf("thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    for(size_t i = 0;  i < 1000;  ++i) {
      // leak this
      void* ptr = HeapAlloc(GetProcessHeap(), 0, i);
      (void) ptr;

      _MCF_thread* thrd = _MCF_thread_new(thread_proc, "hello", 5);
      assert(thrd);

      _MCF_thread_wait(thrd, NULL);
      _MCF_thread_drop_ref(thrd);
    }
  }
