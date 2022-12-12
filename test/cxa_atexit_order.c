/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxa.h"
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
    __MCF_cxa_atexit(cleanup_4, (void*) 4, NULL);
    __MCF_cxa_thread_atexit(cleanup_2, (void*) 2, NULL);
    __MCF_cxa_thread_atexit(cleanup_1, (void*) 1, NULL);
    __MCF_cxa_atexit(cleanup_3, (void*) 3, NULL);
    __MCF_cxa_finalize(NULL);
  }
