/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static
void
cleanup_1(void* ptr)
  {
    assert((intptr_t) ptr == 1);
  }

static
void
cleanup_2(void* ptr)
  {
    assert((intptr_t) ptr == 2);
  }

static
void
cleanup_3(void* ptr)
  {
    assert((intptr_t) ptr == 3);
  }

static
void
cleanup_4(void* ptr)
  {
    assert((intptr_t) ptr == 4);
  }

int
main(void)
  {
    __MCF_cxa_atexit(cleanup_4, (void*) 4, __MCF_nullptr);
    __MCF_cxa_thread_atexit(cleanup_2, (void*) 2, __MCF_nullptr);
    __MCF_cxa_thread_atexit(cleanup_1, (void*) 1, __MCF_nullptr);
    __MCF_cxa_atexit(cleanup_3, (void*) 3, __MCF_nullptr);

    __MCF_cxa_finalize(__MCF_nullptr);
    __MCF__Exit(0);
  }
