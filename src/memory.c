// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_MEMORY_C_  1
#include "memory.h"
#include "win32.h"

void*
_MCF_malloc0(size_t size)
  {
    return HeapAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, size);
  }

void*
_MCF_mrealloc0_ptr(void* ptr, size_t size)
  {
    return HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, ptr, size);
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

unsigned char
_MCF_mequal(const void* src, const void* cmp, size_t size)
  {
    return RtlCompareMemory(src, cmp, size) == size;
  }
