/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_TLS_EXTERN_INLINE
#include "tls.h"
#include "atomic.h"
#include "xwin32.i"

_MCF_tls_key*
_MCF_tls_key_new(_MCF_tls_dtor* dtor_opt)
  {
    _MCF_tls_key* key = __MCF_malloc_0(sizeof(_MCF_tls_key));
    if(!key)
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, NULL);

    /* Initialize the key structure. The returned pointer is assumed to be
     * unique, so its reference count should be initialized to one.  */
    _MCF_atomic_store_32_rlx(key->__nref, 1);
    key->__dtor_opt = dtor_opt;
    return key;
  }

static
void
do_tls_key_drop_ref_nonnull(_MCF_tls_key* key)
  {
    int32_t old_ref = _MCF_atomic_xsub_32_arl(key->__nref, 1);
    __MCFGTHREAD_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    /* Deallocate its storage now.  */
    __MCFGTHREAD_ASSERT(key->__deleted[0] == 1);
    __MCF_mfree(key);
  }

void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* key)
  {
    __MCFGTHREAD_ASSERT(key->__deleted[0] == 0);
    _MCF_atomic_store_8_rlx(key->__deleted, 1);

    do_tls_key_drop_ref_nonnull(key);
  }

static inline
__MCF_tls_element*
do_linear_probe_nonempty(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    __MCFGTHREAD_ASSERT(key);

    /* Keep the load factor no more than 0.5.  */
    uint64_t dist = (uintptr_t) (table->__end - table->__begin);
    __MCFGTHREAD_ASSERT(dist != 0);
    __MCFGTHREAD_ASSERT(table->__size <= dist / 2);

    /* Make a fixed-point value in the interval [0,1), and then multiply
     * `dist` by it to get an index in the middle.  */
    uint32_t ratio = (uint32_t) ((uintptr_t) key / sizeof(void*)) * 0x9E3779B9U;
    __MCF_tls_element* origin = table->__begin + (ptrdiff_t) (dist * ratio >> 32);
    __MCFGTHREAD_ASSERT(origin < table->__end);

    /* Find an element using linear probing.
     * Note this function may return a pointer to an empty element.  */
    for(__MCF_tls_element* cur = origin;  cur != table->__end;  ++cur)
      if(!cur->__key_opt || (cur->__key_opt == key))
        return cur;

    for(__MCF_tls_element* cur = table->__begin;  cur != origin;  ++cur)
      if(!cur->__key_opt || (cur->__key_opt == key))
        return cur;

    __MCF_UNREACHABLE;
  }

void*
__MCF_tls_table_get(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    if(!table->__begin)
      return NULL;

    /* Search for the given key.
     * Note `do_linear_probe_nonempty()` may return an empty element.  */
    __MCF_tls_element* elem = do_linear_probe_nonempty(table, key);
    if(!elem->__key_opt)
      return NULL;

    __MCFGTHREAD_ASSERT(elem->__key_opt == key);
    return elem->__value;
  }

int
__MCF_tls_table_set(__MCF_tls_table* table, _MCF_tls_key* key, const void* value)
  {
    size_t capacity = (size_t) (table->__end - table->__begin);
    if(table->__size >= capacity / 2) {
      /* Allocate a larger table. The number of elements is not changed.  */
      capacity = capacity + capacity / 2 + 17;
      __MCF_tls_element* elem = __MCF_malloc_0(capacity * sizeof(__MCF_tls_element));
      if(!elem)
        return -1;

      __MCF_tls_table temp = *table;
      table->__begin = elem;
      table->__end = elem + capacity;

      /* Relocate existent elements, if any.  */
      if(temp.__begin) {
        while(temp.__begin != temp.__end) {
          temp.__end --;

          /* Skip empty buckets.  */
          _MCF_tls_key* tkey = temp.__end->__key_opt;
          if(!tkey)
            continue;

          if(_MCF_atomic_load_8_rlx(tkey->__deleted) != 0) {
            /* If the key has been deleted, don't relocate it; free it instead.  */
            do_tls_key_drop_ref_nonnull(tkey);
            continue;
          }

          /* Relocate this element into the new storage.  */
          elem = do_linear_probe_nonempty(table, tkey);
          __MCFGTHREAD_ASSERT(!elem->__key_opt);
          *elem = *(temp.__end);
        }

        /* Deallocate the old table which should be empty now.  */
        __MCF_mfree(temp.__begin);
      }
    }

    /* Search for the given key.
     * Note `do_linear_probe_nonempty()` may return an empty element.  */
    __MCF_tls_element* elem = do_linear_probe_nonempty(table, key);
    if(!elem->__key_opt) {
      /* Fill `key` into this element.  */
      int32_t old_ref = _MCF_atomic_xadd_32_arl(key->__nref, 1);
      __MCFGTHREAD_ASSERT(old_ref > 0);
      elem->__key_opt = key;
      table->__size ++;
    }

    __MCFGTHREAD_ASSERT(elem->__key_opt == key);
    elem->__value = (void*) value;
    return 0;
  }

void
__MCF_tls_table_finalize(__MCF_tls_table* table)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_tls_table temp;

    /* The table may be modified by destructors, so swap it out first.  */
    while(table->__begin) {
      temp = *table;
      *table = (__MCF_tls_table) __MCF_0_INIT;

      while(temp.__begin != temp.__end) {
        temp.__end --;

        /* Skip empty buckets.  */
        _MCF_tls_key* tkey = temp.__end->__key_opt;
        if(!tkey)
          continue;

        if(temp.__end->__value) {
          /* Invoke the destructor if there is a value and a destructor, and
           * the key has not been deleted.  */
          _MCF_tls_dtor* dtor = NULL;
          if(_MCF_atomic_load_8_rlx(tkey->__deleted) == 0)
            dtor = tkey->__dtor_opt;

          if(dtor)
            dtor(temp.__end->__value);
        }

        do_tls_key_drop_ref_nonnull(tkey);
      }

      /* Deallocate the table which should be empty now.  */
      __MCF_mfree(temp.__begin);
    }
  }
