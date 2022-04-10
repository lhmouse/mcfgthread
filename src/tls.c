// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_TLS_C_  1
#include "tls.h"
#include "atomic.h"
#include "memory.h"
#include "win32.h"

_MCF_tls_key*
_MCF_tls_key_new(_MCF_tls_dtor* dtor_opt)
  {
    _MCF_tls_key* key = _MCF_malloc0(sizeof(_MCF_tls_key));
    if(!key)
      return NULL;

    // Get the next serial number. Ensure it is non-zero.
    uint32_t serial = UINT32_C(1) << 31;
    serial |= __MCF_ATOMIC_ADD_RLX(&__MCF_tls_key_serial, 1);

    // Initialize the key structure. The returned pointer is assumed to be
    // unique, so its reference count should be initialized to one.
    key->__dtor_opt = dtor_opt;
    __MCF_ATOMIC_STORE_N_RLX(key->__serial, serial);
    __MCF_ATOMIC_STORE_N_RLX(key->__nref, 1);
    return key;
  }

static inline uint32_t
do_get_key_serial(const _MCF_tls_key* key)
  {
    return __MCF_ATOMIC_LOAD_N_RLX(key->__serial);
  }

static void
do_tls_key_drop_ref_nonnull(_MCF_tls_key* key)
  {
    int old_ref = __MCF_ATOMIC_SUB_ARL(key->__nref, 1);
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
    __MCF_ATOMIC_STORE_N_RLX(key->__serial, 0);

    do_tls_key_drop_ref_nonnull(key);
  }

static __MCF_tls_element*
do_linear_probe_nonempty(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    // Keep the load factor no more than 0.5.
    uint64_t dist = (uintptr_t) (table->__end - table->__begin);
    __MCFGTHREAD_ASSERT(dist != 0);
    __MCFGTHREAD_ASSERT(table->__size <= dist / 2);

    // Make a fixed-point value in the interval [0,1), and then multiply
    // `dist` by it to get an index in the middle.
    uint32_t serial = do_get_key_serial(key);
    __MCFGTHREAD_ASSERT(serial != 0);
    dist = dist * (uint32_t) (serial * 0x9E3779B9) >> 32;

    __MCF_tls_element* origin = table->__begin + (ptrdiff_t) dist;
    __MCFGTHREAD_ASSERT(origin < table->__end);

    // Find an element using linear probing.
    // Note this function may return the pointer to an empty element.
    for(__MCF_tls_element* cur = origin;  cur != table->__end;  ++cur)
      if(!cur->__key_opt || (do_get_key_serial(cur->__key_opt) == serial))
        return cur;

    for(__MCF_tls_element* cur = table->__begin;  cur != origin;  ++cur)
      if(!cur->__key_opt || (do_get_key_serial(cur->__key_opt) == serial))
        return cur;

    __MCF_UNREACHABLE;
  }

void*
__MCF_tls_table_get(const __MCF_tls_table* table, const _MCF_tls_key* key)
  {
    __MCFGTHREAD_ASSERT(key);
    if(!table->__begin)
      return NULL;

    // Search for the given key.
    // Note `do_linear_probe_nonempty()` may return an empty element.
    __MCF_tls_element* elem = do_linear_probe_nonempty(table, key);
    if(!elem->__key_opt)
      return NULL;

    __MCFGTHREAD_ASSERT(elem->__key_opt == key);
    return elem->__value;
  }

int
__MCF_tls_table_set(__MCF_tls_table* table, _MCF_tls_key* key, const void* value)
  {
    __MCFGTHREAD_ASSERT(key);
    size_t capacity = (size_t) (table->__end - table->__begin);
    if(table->__size >= capacity / 2) {
      // Allocate a larger table. The number of elements is not changed.
      capacity = capacity + capacity / 2 + 17;
      __MCF_tls_element* elem = _MCF_malloc0(capacity * sizeof(__MCF_tls_element));
      if(!elem)
        return -1;

      __MCF_tls_table temp = *table;
      table->__begin = elem;
      table->__end = elem + capacity;

      while(temp.__begin != temp.__end) {
        temp.__end --;

        // Skip empty elements.
        if(!temp.__end->__key_opt)
          continue;

        // Relocate this element into the new storage.
        elem = do_linear_probe_nonempty(table, temp.__end->__key_opt);
        __MCFGTHREAD_ASSERT(!elem->__key_opt);
        *elem = *(temp.__end);
      }

      // Deallocate the old storage, if amy.
      if(temp.__begin)
        _MCF_mfree_nonnull(temp.__begin);
    }

    // Search for the given key.
    // Note `do_linear_probe_nonempty()` may return an empty element.
    __MCF_tls_element* elem = do_linear_probe_nonempty(table, key);
    if(!elem->__key_opt) {
      int old_ref = __MCF_ATOMIC_ADD_ARL(key->__nref, 1);
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

    for(;;) {
      // The table may be modified while being scanned so swap it out first.
      temp = *table;
      *table = (__MCF_tls_table) { 0 };

      if(!temp.__begin)
        break;

      while(temp.__begin != temp.__end) {
        temp.__end --;

        // Skip empty elements.
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
  }
