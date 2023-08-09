/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_TLS_IMPORT  __MCF_DLLEXPORT
#define __MCF_TLS_INLINE  __MCF_DLLEXPORT
#include "tls.h"
#include "atomic.h"
#include "xglobals.i"

__MCF_DLLEXPORT
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

__MCF_DLLEXPORT
void
_MCF_tls_key_drop_ref_nonnull(_MCF_tls_key* key)
  {
    int32_t old_ref = _MCF_atomic_xsub_32_arl(key->__nref, 1);
    __MCF_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    /* Deallocate all associated resources.  */
    __MCF_mfree(key);
  }

static inline
__MCF_tls_element*
do_linear_probe_nonempty(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    __MCF_ASSERT(key);

    /* Keep the load factor no more than 0.5.  */
    uint64_t dist = (uintptr_t) (table->__end - table->__begin);
    __MCF_ASSERT(dist != 0);
    __MCF_ASSERT(table->__used <= dist / 2);

    /* Make a fixed-point value in the interval [0,1), and then multiply
     * `dist` by it to get an index in the middle.  */
    uint32_t ratio = (uint32_t) ((uintptr_t) key / sizeof(void*)) * 0x9E3779B9U;
    __MCF_tls_element* origin = table->__begin + (ptrdiff_t) (dist * ratio >> 32);
    __MCF_ASSERT(origin < table->__end);

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

__MCF_DLLEXPORT
void*
__MCF_tls_table_get(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    if(_MCF_atomic_load_8_rlx(key->__deleted))
      return NULL;

    if(!table->__begin)
      return NULL;

    /* Search for the given key.
     * Note `do_linear_probe_nonempty()` may return an empty element.  */
    __MCF_tls_element* elem = do_linear_probe_nonempty(table, key);
    if(!elem->__key_opt)
      return NULL;

    __MCF_ASSERT(elem->__key_opt == key);
    return elem->__value_opt;
  }

__MCF_DLLEXPORT
int
__MCF_tls_table_xset(__MCF_tls_table* table, _MCF_tls_key* key, void** old_value_opt, const void* value_opt)
  {
    __MCF_SET_IF(old_value_opt, NULL);

    if(_MCF_atomic_load_8_rlx(key->__deleted))
      return -1;

    if(!value_opt && !table->__begin) {
      /* The new value will be effectively unset. If the table is empty, there
       * can't be a value to unset, so report success anyway.  */
      return 0;
    }

    if(value_opt && (table->__used >= (size_t) (table->__end - table->__begin) / 2)) {
      /* Allocate a larger table in case that a new value is to be inserted.
       * The number of elements is unchanged.  */
      size_t capacity = table->__used * 3 | 17;
      __MCF_tls_element* elem = __MCF_malloc_0(capacity * sizeof(__MCF_tls_element));
      if(!elem)
        return -1;

      __MCF_tls_table temp = *table;
      table->__begin = elem;
      table->__end = elem + capacity;
      table->__used = 0;

      while(temp.__begin != temp.__end) {
        temp.__end --;

        /* Skip empty buckets.  */
        _MCF_tls_key* tkey = temp.__end->__key_opt;
        if(!tkey)
          continue;

        if(_MCF_atomic_load_8_rlx(tkey->__deleted)) {
          /* If the key has been deleted, don't relocate it; free it.  */
          _MCF_tls_key_drop_ref_nonnull(tkey);
          continue;
        }

        /* Relocate this element into the new storage.  */
        elem = do_linear_probe_nonempty(table, tkey);
        __MCF_ASSERT(!elem->__key_opt);
        elem->__key_opt = tkey;
        elem->__value_opt = temp.__end->__value_opt;
        table->__used ++;
      }

      /* Deallocate the old table which should be empty now.  */
      __MCF_mfree(temp.__begin);
    }

    /* Search for the given key.
     * Note `do_linear_probe_nonempty()` may return an empty element.  */
    __MCF_tls_element* elem = do_linear_probe_nonempty(table, key);
    if(!elem->__key_opt) {
      /* If `key` is not found and a null pointer is to be set, don't
       * actually set it; report success.  */
      if(!value_opt)
        return 0;

      /* Fill `key` into this element.  */
      _MCF_atomic_xadd_32_arl(key->__nref, 1);
      elem->__key_opt = key;
      table->__used ++;
    }

    __MCF_ASSERT(elem->__key_opt == key);
    __MCF_SET_IF(old_value_opt, elem->__value_opt);
    elem->__value_opt = (void*) value_opt;
    return 0;
  }
