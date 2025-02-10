/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>

static
int
test_thrd_func(void* p)
  {
    thrd_sleep(&(struct timespec) { .tv_sec = 2 }, __MCF_nullptr);
    thrd_t mthr = p;
    fprintf(stderr, "got main thread = %p, ref = %d\n", (void*) mthr, _MCF_thread_get_ref(mthr));

    int code;
    int err = thrd_join(mthr, &code);
    assert(err == 0);
    fprintf(stderr, "main thread joined: code = %d\n", code);
    // zero; main thread is foreign.
    assert(code == 0);
    __MCF__Exit(0);
  }

int
main(void)
  {
    thrd_t mthr = thrd_current();
    assert(mthr);
    fprintf(stderr, "main thread = %p, ref = %d\n", (void*) mthr, _MCF_thread_get_ref(mthr));

    thrd_t cthr;
    int err = thrd_create(&cthr, test_thrd_func, mthr);
    assert(err == 0);
    fprintf(stderr, "new thread = %p\n", (void*) cthr);

    thrd_sleep(&(struct timespec) { .tv_sec = 1 }, __MCF_nullptr);
    fprintf(stderr, "main thread exiting\n");
    thrd_exit(42);
  }
