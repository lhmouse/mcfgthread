// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_TLS_C_  1
#include "tls.h"
#include "memory.h"
#include "win32.h"

_MCF_tls_key*
_MCF_tls_key_new(_MCF_tls_dtor* dtor_opt)
  {
    _MCF_tls_key* key = _MCF_malloc0(sizeof(_MCF_tls_key));
    if(!key)
      return NULL;

    // Get the next serial number. Ensure it is non-zero.
    uint32_t serial = __atomic_fetch_add(&__MCF_tls_key_serial, 1, __ATOMIC_RELAXED);
    serial = serial * 2 + 1;

    // Initialize the key structure. The returned pointer is assumed to be
    // unique, so its reference count should be initialized to one.
    key->__dtor_opt = dtor_opt;
    __atomic_store_n(key->__serial, serial, __ATOMIC_RELAXED);
    __atomic_store_n(key->__nref, 1, __ATOMIC_RELAXED);
    return key;
  }

static inline uint32_t
do_get_key_serial(const _MCF_tls_key* key)
  {
    return __atomic_load_n(key->__serial, __ATOMIC_RELAXED);
  }

static void
do_tls_key_drop_ref_nonnull(_MCF_tls_key* key)
  {
    int old_ref = __atomic_fetch_sub(key->__nref, 1, __ATOMIC_ACQ_REL);
    __MCFGTHREAD_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    // Deallocate its storage now.
    __MCFGTHREAD_ASSERT(do_get_key_serial(key) == 0);
    _MCF_mfree(key);
  }

void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* key)
  {
    __MCFGTHREAD_ASSERT(do_get_key_serial(key) != 0);
    __atomic_store_n(key->__serial, 0, __ATOMIC_RELAXED);

    do_tls_key_drop_ref_nonnull(key);
  }

static __MCF_tls_element*
do_probe_table(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    uint64_t dist = (uintptr_t) (table->__end - table->__begin);
    if(dist == 0)
      return NULL;

    // Make a fixed-point value in the interval [0,1), and then multiply
    // `dist` by it to get an index in the middle.
    uint32_t serial = do_get_key_serial(key);
    __MCFGTHREAD_ASSERT(serial != 0);
    dist = dist * (uint32_t) (serial * 0x9E3779B9) >> 32;

    __MCF_tls_element* origin = table->__begin + (uintptr_t) dist;
    __MCFGTHREAD_ASSERT(origin < table->__end);

    // Find an element using linear probing.
    // Note this function may return the pointer to an empty element.
    for(__MCF_tls_element* cur = origin;  cur != table->__end;  ++cur)
      if((cur->__key_opt == NULL) || (do_get_key_serial(cur->__key_opt) == serial))
        return cur;

    for(__MCF_tls_element* cur = table->__begin;  cur != origin;  ++cur)
      if((cur->__key_opt == NULL) || (do_get_key_serial(cur->__key_opt) == serial))
        return cur;

    // The table is full and no desired element has been found so far.
    return NULL;
  }

void*
__MCF_tls_table_get(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    __MCF_tls_element* elem = do_probe_table(table, key);
    if(!elem)
      return NULL;

    // Note `do_probe_table()` may return an empty element.
    if(elem->__key_opt == NULL)
      return NULL;

    __MCFGTHREAD_ASSERT(elem->__key_opt == key);
    return elem->__value;
  }

int
__MCF_tls_table_set(__MCF_tls_table* table, _MCF_tls_key* key, void* value)
  {
    __MCF_tls_element* elem = do_probe_table(table, key);
    if(!elem) {
      // The table is full, so let's allocate a larger one.
      // We extend the table by a half.
      size_t new_capacity = (size_t) (table->__end - table->__begin) * 2 + 17;
      new_capacity = new_capacity + new_capacity / 2 + 17;
      elem = _MCF_malloc0(new_capacity * sizeof(__MCF_tls_element));
      if(!elem)
        return -1;

      // Replace the table.
      __MCF_tls_table temp = *table;
      table->__begin = elem;
      table->__end = elem + new_capacity;

      while(temp.__begin != temp.__end) {
        temp.__end --;

        // Skip 'holes'.
        if(temp.__end->__key_opt == NULL)
          continue;

        // Relocate this element into the new table.
        elem = do_probe_table(table, temp.__end->__key_opt);
        __MCFGTHREAD_ASSERT(elem);
        __MCFGTHREAD_ASSERT(elem->__key_opt == NULL);
        *elem = *(temp.__end);
      }

      // Deallocate the old storage, if amy.
      _MCF_mfree(temp.__begin);

      // Search for an empty slot again. This will not fail.
      elem = do_probe_table(table, key);
      __MCFGTHREAD_ASSERT(elem);
      __MCFGTHREAD_ASSERT(elem->__key_opt == NULL);
    }

    if(elem->__key_opt == NULL) {
      // Initialize this element.
      int old_ref = __atomic_fetch_add(key->__nref, 1, __ATOMIC_ACQ_REL);
      __MCFGTHREAD_ASSERT(old_ref > 0);
      elem->__key_opt = key;
    }

    __MCFGTHREAD_ASSERT(elem->__key_opt == key);
    elem->__value = value;
    return 0;
  }

void
__MCF_tls_table_finalize(__MCF_tls_table* table)
  {
    __MCF_SEH_TERMINATE_FILTER_BEGIN
    __MCF_tls_table temp;

    for(;;) {
      // The table may be modified while we are iterating so swap it out first.
      temp = *table;
      *table = (__MCF_tls_table) { 0 };

      if(!temp.__begin)
        break;

      while(temp.__begin != temp.__end) {
        temp.__end --;

        // Skip 'holes'.
        if(temp.__end->__key_opt == NULL)
          continue;

        // Invoke the destructor if there is a value and a destructor, and the
        // key has not been deleted.
        _MCF_tls_key* key = temp.__end->__key_opt;

        if(temp.__end->__value && key->__dtor_opt && do_get_key_serial(key))
          key->__dtor_opt(temp.__end->__value);

        do_tls_key_drop_ref_nonnull(key);
      }

      // Deallocate the table which should be empty now.
      _MCF_mfree_nonnull(temp.__begin);
    }

    __MCF_SEH_TERMINATE_FILTER_END
  }
