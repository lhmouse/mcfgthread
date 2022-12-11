/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/c11.h"
#include <assert.h>

int
main(void)
  {
// FIXME: We need to figure out how to simulate this first...
// See https://github.com/lhmouse/mcfgthread/issues/73.
return 77;

    volatile intptr_t mcfp, gthp;

    mcfp = (intptr_t) __MCF_c11_call_once;
    gthp = (intptr_t) call_once;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_cnd_broadcast;
    gthp = (intptr_t) cnd_broadcast;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_cnd_destroy;
    gthp = (intptr_t) cnd_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_cnd_init;
    gthp = (intptr_t) cnd_init;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_cnd_signal;
    gthp = (intptr_t) cnd_signal;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_cnd_timedwait;
    gthp = (intptr_t) cnd_timedwait;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_cnd_wait;
    gthp = (intptr_t) cnd_wait;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_mtx_destroy;
    gthp = (intptr_t) mtx_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_mtx_init;
    gthp = (intptr_t) mtx_init;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_mtx_lock;
    gthp = (intptr_t) mtx_lock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_mtx_timedlock;
    gthp = (intptr_t) mtx_timedlock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_mtx_trylock;
    gthp = (intptr_t) mtx_trylock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_mtx_unlock;
    gthp = (intptr_t) mtx_unlock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_create;
    gthp = (intptr_t) thrd_create;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_current;
    gthp = (intptr_t) thrd_current;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_detach;
    gthp = (intptr_t) thrd_detach;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_equal;
    gthp = (intptr_t) thrd_equal;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_exit;
    gthp = (intptr_t) thrd_exit;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_join;
    gthp = (intptr_t) thrd_join;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_sleep;
    gthp = (intptr_t) thrd_sleep;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_thrd_yield;
    gthp = (intptr_t) thrd_yield;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_tss_create;
    gthp = (intptr_t) tss_create;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_tss_delete;
    gthp = (intptr_t) tss_delete;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_tss_get;
    gthp = (intptr_t) tss_get;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_c11_tss_set;
    gthp = (intptr_t) tss_set;
    assert(mcfp == gthp);

    return 0;
  }
