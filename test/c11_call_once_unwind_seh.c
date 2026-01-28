/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#define NTHREADS  64U
static thrd_t threads[NTHREADS];
static once_flag once = ONCE_FLAG_INIT;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
EXCEPTION_DISPOSITION
unwind_done(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    if(rec->ExceptionCode == 0xC0000027)  // STATUS_UNWIND
      ExitThread(0);

    return ExceptionContinueSearch;
  }

static
void
once_do_it(void)
  {
    /* Perform initialization.  */
    int old = resource;
    _MCF_sleep((const int64_t[]) { -20 });
    resource = old + 1;

    _MCF_sleep((const int64_t[]) { -10 });
    fprintf(stderr, "thread %d once\n", (int) _MCF_thread_self_tid());

    RtlUnwind(__MCF_nullptr, __MCF_nullptr, __MCF_nullptr, __MCF_nullptr);
    abort();
  }

static
int
thread_proc(void* param)
  {
#ifdef __i386__
    EXCEPTION_REGISTRATION_RECORD record;
    __MCF_TEB_LOAD_32_ABS(&(record.Next), 0);
    record.Handler = (EXCEPTION_ROUTINE*)(DWORD) unwind_done;
    __MCF_TEB_STORE_32_ABS(0, &record);
#else
    __asm__ (".seh_handler %c0, @except, @unwind" : : "i"(unwind_done));
#endif

    (void) param;
    _MCF_sem_wait(&start, __MCF_nullptr);

    call_once(&once, once_do_it);
    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    ExitThread(0);
  }

int
main(void)
  {
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_create(&threads[k], thread_proc, __MCF_nullptr);
      assert(r == thrd_success);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    _MCF_sem_signal_some(&start, NTHREADS);
    for(size_t k = 0;  k < NTHREADS;  ++k) {
      int r = thrd_join(threads[k], __MCF_nullptr);
      assert(r == thrd_success);
      fprintf(stderr, "main wait finished: %d\n", (int)k);
    }

    assert(resource == NTHREADS);
  }
