// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_MEMORY_H_
#define __MCFGTHREAD_MEMORY_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCFGTHREAD_MEMORY_C_
#  define __MCFGTHREAD_MEMORY_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_MEMORY_INLINE
#endif

// Allocate a block of memory, like `calloc()`.
void*
_MCF_malloc0(size_t __size) __MCF_NOEXCEPT __attribute__((__malloc__));

// Get the size of an allocated block, like `malloc_usable_size()`.
size_t
_MCF_msize(void* __ptr) __MCF_NOEXCEPT;

// Free a block of memory, like `free()`.
void
_MCF_mfree(void* __ptr) __MCF_NOEXCEPT;

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
unsigned char
_MCF_mequal(const void* __src, const void* __cmp, size_t __size) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_MEMORY_H_
