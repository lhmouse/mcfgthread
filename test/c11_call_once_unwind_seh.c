/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#include <windows.h>
#include <ntstatus.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];
static once_flag once = ONCE_FLAG_INIT;
static _MCF_sem start = __MCF_SEM_INIT(0);
static int resource = 0;

static
EXCEPTION_DISPOSITION
unwind_done(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    if(rec->ExceptionCode == (DWORD) STATUS_UNWIND)
      ExitThread(0);

    return ExceptionContinueSearch;
  }

#if defined __i386__ && defined _MSC_VER
__asm__ (".safeseh _unwind_done");
#endif

static
void
once_do_it(void)
  {
    /* Perform initialization.  */
    int old = resource;
    _MCF_sleep_noninterruptible(&(int64_t){ -20 });
    resource = old + 1;

    _MCF_sleep_noninterruptible(&(int64_t){ -10 });
    fprintf(stderr, "thread %d once\n", __MCF_tid());

    RtlUnwind(NULL, NULL, NULL, NULL);
    abort();
  }

static
int
thread_proc(void* param)
  {
#ifdef __i386__
    DWORD record[] = { (DWORD) __MCF_teb_load_ptr(0), (DWORD) unwind_done };
    __MCF_teb_store_ptr(0, (intptr_t) &record);
#else
    __asm__ (".seh_handler %c0, @except, @unwind" : : "i"(unwind_done));
#endif

    (void) param;
    _MCF_sem_wait(&start, NULL);

    call_once(&once, once_do_it);
    fprintf(stderr, "thread %d quitting\n", __MCF_tid());
    ExitThread(0);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, NULL);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], NULL);
      assert(r == thrd_success);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
