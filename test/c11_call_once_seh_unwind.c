/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
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

#include "../mcfgthread/c11.h"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>

#define NTHREADS  32U
static thrd_t threads[NTHREADS];
static once_flag once = ONCE_FLAG_INIT;
static _MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

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

    /* effect access violation.  */
    *__MCF_CAST_PTR(int, once_do_it) = 42;
    fprintf(stderr, "never here\n");
  }

static
void
__stdcall
tls_callback(void* instance, unsigned long reason, void* reserved)
  {
    (void) instance;
    (void) reserved;

    /* do test if `DLL_THREAD_DETACH`; exceptions are swallowed by the system.  */
    if(reason == 3)
      call_once(&once, once_do_it);
  }

#if defined _MSC_VER
__pragma(section(".CRT$XLY", read))
__pragma(comment(linker, "/include:" __MCF_USYM "_tls_used"))
#endif

__attribute__((__section__(".CRT$XLY"), __used__))
__typeof__(tls_callback)* my_tls_callback = tls_callback;

static
int
thread_proc(void* param)
  {
    (void) param;
    _MCF_sem_wait(&start, __MCF_nullptr);

    /* do test in TLS callback.  */
    fprintf(stderr, "thread %d quitting\n", (int) _MCF_thread_self_tid());
    return 0;
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

#endif  // __CYGWIN__
