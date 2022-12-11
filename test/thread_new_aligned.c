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
    for(size_t i = 0;  i < 32;  ++i) {
      // leak this
      void* ptr = HeapAlloc(GetProcessHeap(), 0, 1);
      (void) ptr;

      uint32_t align = 1U << i;
      uint32_t size = 0x10000U;
      _MCF_thread* thrd = _MCF_thread_new_aligned(thread_proc, align, NULL, size);
      if(!thrd) {
        assert(GetLastError() == ERROR_NOT_SUPPORTED);
        continue;
      }

      void* data = _MCF_thread_get_data(thrd);
      assert((uintptr_t) data % align == 0);
      assert((char*) data + size <= (char*) thrd + HeapSize(GetProcessHeap(), 0, thrd));

      _MCF_thread_wait(thrd, NULL);
      _MCF_thread_drop_ref(thrd);
    }
  }
