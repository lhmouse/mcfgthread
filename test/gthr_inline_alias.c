/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/gthr.h"
#include <assert.h>

int
main(void)
  {
    __MCF_STATIC_ASSERT(__MCF_gthr_active_p == __gthread_active_p);
    __MCF_STATIC_ASSERT(__MCF_gthr_getspecific == __gthread_getspecific);
    __MCF_STATIC_ASSERT(__MCF_gthr_key_create == __gthread_key_create);
    __MCF_STATIC_ASSERT(__MCF_gthr_key_delete == __gthread_key_delete);
    __MCF_STATIC_ASSERT(__MCF_gthr_mutex_destroy == __gthread_mutex_destroy);
    __MCF_STATIC_ASSERT(__MCF_gthr_mutex_lock == __gthread_mutex_lock);
    __MCF_STATIC_ASSERT(__MCF_gthr_mutex_trylock == __gthread_mutex_trylock);
    __MCF_STATIC_ASSERT(__MCF_gthr_mutex_unlock == __gthread_mutex_unlock);
    __MCF_STATIC_ASSERT(__MCF_gthr_once == __gthread_once);
    __MCF_STATIC_ASSERT(__MCF_gthr_recursive_mutex_destroy == __gthread_recursive_mutex_destroy);
    __MCF_STATIC_ASSERT(__MCF_gthr_recursive_mutex_lock == __gthread_recursive_mutex_lock);
    __MCF_STATIC_ASSERT(__MCF_gthr_recursive_mutex_trylock == __gthread_recursive_mutex_trylock);
    __MCF_STATIC_ASSERT(__MCF_gthr_recursive_mutex_unlock == __gthread_recursive_mutex_unlock);
    __MCF_STATIC_ASSERT(__MCF_gthr_setspecific == __gthread_setspecific);

    return 0;
  }
