/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    volatile intptr_t mcfp, gthp;

    mcfp = (intptr_t) __MCF_gthr_mutex_lock;
    printf("mcfp = %tx\n", mcfp);

    gthp = (intptr_t) __gthread_mutex_lock;
    printf("gthp = %tx\n", gthp);

    assert(mcfp = gthp);

    return 0;
  }
