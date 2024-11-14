/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#if defined __CYGWIN__
int
main(void)
  {
    return 77;
  }
#else  // __CYGWIN__

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#define __MCF_XGLOBALS_IMPORT
#define __MCF_XGLOBALS_INLINE  __MCF_GNU_INLINE
#define __MCF_XGLOBALS_READONLY
#include "../mcfgthread/xglobals.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static HANDLE threads[NTHREADS];

static
DWORD
__stdcall
thread_proc(void* arg)
  {
    (void) arg;
    Sleep(1000);
    assert(_MCF_thread_self() == &(__MCF_g->__thread_oom_self_st));
    Sleep(100);
    fprintf(stderr, "thread %d quitting\n", _MCF_thread_self_tid());
    return 0;
  }

int
main(void)
  {
    // replace global heap
    const uint32_t heap_capacity = 1048576;
    __MCF_crt_heap = HeapCreate(0, 0, heap_capacity);
    assert(&__MCF_crt_heap);

    // allocate until oom
    for(uint32_t i = 0;  i < heap_capacity / 16;  ++i)
      (void)! __MCF_malloc_0(16);

    // create threads; must be native
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      threads[k] = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      WaitForSingleObject(threads[k], INFINITE);
      CloseHandle(threads[k]);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }
  }

#endif  // __CYGWIN__
