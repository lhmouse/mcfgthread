/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
#include <assert.h>

int
main(void)
  {
    __MCF_STATIC_ASSERT(__MCF_c11_call_once == call_once);
    __MCF_STATIC_ASSERT(__MCF_c11_cnd_broadcast == cnd_broadcast);
    __MCF_STATIC_ASSERT(__MCF_c11_cnd_destroy == cnd_destroy);
    __MCF_STATIC_ASSERT(__MCF_c11_cnd_init == cnd_init);
    __MCF_STATIC_ASSERT(__MCF_c11_cnd_signal == cnd_signal);
    __MCF_STATIC_ASSERT(__MCF_c11_cnd_timedwait == cnd_timedwait);
    __MCF_STATIC_ASSERT(__MCF_c11_cnd_wait == cnd_wait);
    __MCF_STATIC_ASSERT(__MCF_c11_mtx_destroy == mtx_destroy);
    __MCF_STATIC_ASSERT(__MCF_c11_mtx_init == mtx_init);
    __MCF_STATIC_ASSERT(__MCF_c11_mtx_lock == mtx_lock);
    __MCF_STATIC_ASSERT(__MCF_c11_mtx_timedlock == mtx_timedlock);
    __MCF_STATIC_ASSERT(__MCF_c11_mtx_trylock == mtx_trylock);
    __MCF_STATIC_ASSERT(__MCF_c11_mtx_unlock == mtx_unlock);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_create == thrd_create);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_current == thrd_current);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_detach == thrd_detach);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_equal == thrd_equal);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_exit == thrd_exit);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_join == thrd_join);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_sleep == thrd_sleep);
    __MCF_STATIC_ASSERT(__MCF_c11_thrd_yield == thrd_yield);
    __MCF_STATIC_ASSERT(__MCF_c11_tss_create == tss_create);
    __MCF_STATIC_ASSERT(__MCF_c11_tss_delete == tss_delete);
    __MCF_STATIC_ASSERT(__MCF_c11_tss_get == tss_get);
    __MCF_STATIC_ASSERT(__MCF_c11_tss_set == tss_set);

    return 0;
  }
