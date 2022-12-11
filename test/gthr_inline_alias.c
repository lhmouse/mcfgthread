/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/gthr.h"
#include <assert.h>

int
main(void)
  {
    volatile intptr_t mcfp, gthp;

    mcfp = (intptr_t) __MCF_gthr_active_p;
    gthp = (intptr_t) __gthread_active_p;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_getspecific;
    gthp = (intptr_t) __gthread_getspecific;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_key_create;
    gthp = (intptr_t) __gthread_key_create;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_key_delete;
    gthp = (intptr_t) __gthread_key_delete;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_mutex_destroy;
    gthp = (intptr_t) __gthread_mutex_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_mutex_lock;
    gthp = (intptr_t) __gthread_mutex_lock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_mutex_trylock;
    gthp = (intptr_t) __gthread_mutex_trylock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_mutex_unlock;
    gthp = (intptr_t) __gthread_mutex_unlock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_once;
    gthp = (intptr_t) __gthread_once;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_recursive_mutex_destroy;
    gthp = (intptr_t) __gthread_recursive_mutex_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_recursive_mutex_lock;
    gthp = (intptr_t) __gthread_recursive_mutex_lock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_recursive_mutex_trylock;
    gthp = (intptr_t) __gthread_recursive_mutex_trylock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_recursive_mutex_unlock;
    gthp = (intptr_t) __gthread_recursive_mutex_unlock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_gthr_setspecific;
    gthp = (intptr_t) __gthread_setspecific;
    assert(mcfp == gthp);

    return 0;
  }
