/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_MEMORY_EXTERN_INLINE
#include "memory.h"
#include "win32.h"

void*
_MCF_malloc_0(size_t size)
  {
    return HeapAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, size);
  }

void*
_MCF_mprealloc_0(void* ptr, size_t size)
  {
    return HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, ptr, size);
  }

void*
_MCF_malloc_copy(const void* data, size_t size)
  {
    void* ptr = HeapAlloc(__MCF_crt_heap, 0, size);
    if(!ptr)
      return NULL;

    RtlMoveMemory(ptr, data, size);
    return ptr;
  }

void
_MCF_mfree_nonnull(void* ptr)
  {
    __MCFGTHREAD_ASSERT(ptr);
#ifdef __MCF_DEBUG
    RtlFillMemory(ptr, _MCF_msize(ptr), 0xFE);
#endif
    BOOL success = HeapFree(__MCF_crt_heap, 0, ptr);
    __MCFGTHREAD_ASSERT(success);
  }

size_t
_MCF_msize(const void* ptr)
  {
    __MCFGTHREAD_ASSERT(ptr);
    return HeapSize(__MCF_crt_heap, 0, ptr);
  }

void
_MCF_mmove(void* dst, const void* src, size_t size)
  {
    RtlMoveMemory(dst, src, size);
  }

void
_MCF_mfill(void* dst, int val, size_t size)
  {
    RtlFillMemory(dst, size, val);
  }

void
_MCF_mzero(void* dst, size_t size)
  {
    RtlZeroMemory(dst, size);
  }

uint8_t
_MCF_mequal(const void* src, const void* cmp, size_t size)
  {
    return RtlCompareMemory(src, cmp, size) == size;
  }
