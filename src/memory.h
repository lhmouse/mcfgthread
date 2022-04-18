// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_MEMORY_H_
#define __MCFGTHREAD_MEMORY_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_MEMORY_EXTERN_INLINE
#  define __MCF_MEMORY_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

// Allocate a block of zeroed memory, like `calloc()`.
void*
_MCF_malloc0(size_t __size) __MCF_NOEXCEPT
  __attribute__((__malloc__, __alloc_size__(1)));

// Re-allocate a block of memory, like `realloc()`. If the existent
// block should be extended, vacuum bytes are filled with zeroes.
// The result is a boolean value.
uint8_t
_MCF_mrealloc0(void** __pptr, size_t __size) __MCF_NOEXCEPT;

void*
_MCF_mrealloc0_ptr(void* __ptr, size_t __size) __MCF_NOEXCEPT
  __attribute__((__alloc_size__(2)));

__MCF_MEMORY_EXTERN_INLINE uint8_t
_MCF_mrealloc0(void** __pptr, size_t __size) __MCF_NOEXCEPT
  {
    void* __ptr_new = _MCF_mrealloc0_ptr(*__pptr, __size);
    if(!__ptr_new)
      return 0;

    *__pptr = __ptr_new;
    return 1;
  }

// Allocate a copy of a block of memory, like `malloc()` followed by
// `memcpy()`.
void*
_MCF_malloc_copy(const void* __data, size_t __size) __MCF_NOEXCEPT
  __attribute__((__alloc_size__(2)));

// Free a block of memory, like `free()`.
void
_MCF_mfree(void* __ptr_opt) __MCF_NOEXCEPT;

void
_MCF_mfree_nonnull(void* __ptr) __MCF_NOEXCEPT;

__MCF_MEMORY_EXTERN_INLINE void
_MCF_mfree(void* __ptr_opt) __MCF_NOEXCEPT
  {
    if(__ptr_opt)
      _MCF_mfree_nonnull(__ptr_opt);
  }

// Get the size of an allocated block, like `malloc_usable_size()`.
size_t
_MCF_msize(const void* __ptr) __MCF_NOEXCEPT
  __attribute__((__pure__));

// Copy a block of memory, like `memmove()`.
void
_MCF_mmove(void* __dst, const void* __src, size_t __size) __MCF_NOEXCEPT;

// Fill a block of memory with the given byte, like `memset()`.
void
_MCF_mfill(void* __dst, int __val, size_t __size) __MCF_NOEXCEPT;

// Fill a block of memory with zeroes, like `bzero()`.
void
_MCF_mzero(void* __dst, size_t __size) __MCF_NOEXCEPT;

// Check whether two blocks of memory compare equal, like `memcmp() == 0`.
// The result is a boolean value.
uint8_t
_MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT
  __attribute__((__pure__));

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_MEMORY_H_
