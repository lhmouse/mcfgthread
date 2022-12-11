/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/libcxx.h"
#include <assert.h>

int
main(void)
  {
    volatile intptr_t mcfp, gthp;

    mcfp = (intptr_t) __MCF_libcxx_condvar_broadcast;
    gthp = (intptr_t) __libcpp_condvar_broadcast;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_condvar_destroy;
    gthp = (intptr_t) __libcpp_condvar_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_condvar_init;
    gthp = (intptr_t) __libcpp_condvar_init;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_condvar_signal;
    gthp = (intptr_t) __libcpp_condvar_signal;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_condvar_timedwait;
    gthp = (intptr_t) __libcpp_condvar_timedwait;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_condvar_wait;
    gthp = (intptr_t) __libcpp_condvar_wait;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_execute_once;
    gthp = (intptr_t) __libcpp_execute_once;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_mutex_destroy;
    gthp = (intptr_t) __libcpp_mutex_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_mutex_init;
    gthp = (intptr_t) __libcpp_mutex_init;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_mutex_lock;
    gthp = (intptr_t) __libcpp_mutex_lock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_mutex_trylock;
    gthp = (intptr_t) __libcpp_mutex_trylock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_mutex_unlock;
    gthp = (intptr_t) __libcpp_mutex_unlock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_recursive_mutex_destroy;
    gthp = (intptr_t) __libcpp_recursive_mutex_destroy;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_recursive_mutex_init;
    gthp = (intptr_t) __libcpp_recursive_mutex_init;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_recursive_mutex_lock;
    gthp = (intptr_t) __libcpp_recursive_mutex_lock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_recursive_mutex_trylock;
    gthp = (intptr_t) __libcpp_recursive_mutex_trylock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_recursive_mutex_unlock;
    gthp = (intptr_t) __libcpp_recursive_mutex_unlock;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_create;
    gthp = (intptr_t) __libcpp_thread_create;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_detach;
    gthp = (intptr_t) __libcpp_thread_detach;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_get_current_id;
    gthp = (intptr_t) __libcpp_thread_get_current_id;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_get_id;
    gthp = (intptr_t) __libcpp_thread_get_id;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_id_equal;
    gthp = (intptr_t) __libcpp_thread_id_equal;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_id_less;
    gthp = (intptr_t) __libcpp_thread_id_less;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_isnull;
    gthp = (intptr_t) __libcpp_thread_isnull;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_join;
    gthp = (intptr_t) __libcpp_thread_join;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_thread_yield;
    gthp = (intptr_t) __libcpp_thread_yield;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_tls_create;
    gthp = (intptr_t) __libcpp_tls_create;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_tls_delete;
    gthp = (intptr_t) __libcpp_tls_delete;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_tls_get;
    gthp = (intptr_t) __libcpp_tls_get;
    assert(mcfp == gthp);

    mcfp = (intptr_t) __MCF_libcxx_tls_set;
    gthp = (intptr_t) __libcpp_tls_set;
    assert(mcfp == gthp);

    return 0;
  }
