/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/libcxx.h"
#include <assert.h>

int
main(void)
  {
    __MCF_STATIC_ASSERT(__MCF_libcxx_condvar_broadcast == __libcpp_condvar_broadcast);
    __MCF_STATIC_ASSERT(__MCF_libcxx_condvar_destroy == __libcpp_condvar_destroy);
    __MCF_STATIC_ASSERT(__MCF_libcxx_condvar_init == __libcpp_condvar_init);
    __MCF_STATIC_ASSERT(__MCF_libcxx_condvar_signal == __libcpp_condvar_signal);
    __MCF_STATIC_ASSERT(__MCF_libcxx_condvar_timedwait == __libcpp_condvar_timedwait);
    __MCF_STATIC_ASSERT(__MCF_libcxx_condvar_wait == __libcpp_condvar_wait);
    __MCF_STATIC_ASSERT(__MCF_libcxx_execute_once == __libcpp_execute_once);
    __MCF_STATIC_ASSERT(__MCF_libcxx_mutex_destroy == __libcpp_mutex_destroy);
    __MCF_STATIC_ASSERT(__MCF_libcxx_mutex_init == __libcpp_mutex_init);
    __MCF_STATIC_ASSERT(__MCF_libcxx_mutex_lock == __libcpp_mutex_lock);
    __MCF_STATIC_ASSERT(__MCF_libcxx_mutex_trylock == __libcpp_mutex_trylock);
    __MCF_STATIC_ASSERT(__MCF_libcxx_mutex_unlock == __libcpp_mutex_unlock);
    __MCF_STATIC_ASSERT(__MCF_libcxx_recursive_mutex_destroy == __libcpp_recursive_mutex_destroy);
    __MCF_STATIC_ASSERT(__MCF_libcxx_recursive_mutex_init == __libcpp_recursive_mutex_init);
    __MCF_STATIC_ASSERT(__MCF_libcxx_recursive_mutex_lock == __libcpp_recursive_mutex_lock);
    __MCF_STATIC_ASSERT(__MCF_libcxx_recursive_mutex_trylock == __libcpp_recursive_mutex_trylock);
    __MCF_STATIC_ASSERT(__MCF_libcxx_recursive_mutex_unlock == __libcpp_recursive_mutex_unlock);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_create == __libcpp_thread_create);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_detach == __libcpp_thread_detach);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_get_current_id == __libcpp_thread_get_current_id);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_get_id == __libcpp_thread_get_id);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_id_equal == __libcpp_thread_id_equal);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_id_less == __libcpp_thread_id_less);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_isnull == __libcpp_thread_isnull);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_join == __libcpp_thread_join);
    __MCF_STATIC_ASSERT(__MCF_libcxx_thread_yield == __libcpp_thread_yield);
    __MCF_STATIC_ASSERT(__MCF_libcxx_tls_create == __libcpp_tls_create);
    __MCF_STATIC_ASSERT(__MCF_libcxx_tls_delete == __libcpp_tls_delete);
    __MCF_STATIC_ASSERT(__MCF_libcxx_tls_get == __libcpp_tls_get);
    __MCF_STATIC_ASSERT(__MCF_libcxx_tls_set == __libcpp_tls_set);

    return 0;
  }
