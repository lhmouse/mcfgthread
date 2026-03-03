/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_TEB_
#define __MCFGTHREAD_TEB_

#include "fwd.h"
#if defined _MSC_VER && !defined __clang__
#include <intrin.h>
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_TEB_IMPORT
#  define __MCF_TEB_IMPORT
#  define __MCF_TEB_INLINE  __MCF_ALWAYS_INLINE
#endif

__MCF_TEB_INLINE __MCF_FN_PURE
uint32_t
__MCF_teb_load_32(uint32_t __base, uint32_t __index)
  __MCF_noexcept
  {
    uint32_t __value;
#if defined __MCF_M_X8664_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%gs:%a1, %k0 | %k0, gs:[%1] }"
        : "=r"(__value) : "i"(__base + __index * 4)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ (
        "mov { %%gs:%a1(,%2,4), %k0 | %k0, gs:[%1+%2*4] }"
        : "=r"(__value) : "i"(__base), "r"((uint64_t) __index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%gs:%a2(%1), %k0 | %k0, gs:[%1+%2] }"
        : "=r"(__value) : "r"((uint64_t) __base), "i"(__index * 4)
        : "memory"
      );
    else
      __asm__ (
        "mov { %%gs:(%1,%2,4), %k0 | %k0, gs:[%1+%2*4] }"
        : "=r"(__value) : "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_X8632_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%fs:%a1, %k0 | %k0, fs:[%1] }"
        : "=r"(__value) : "i"(__base + __index * 4)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ (
        "mov { %%fs:%a1(,%2,4), %k0 | %k0, fs:[%1+%2*4] }"
        : "=r"(__value) : "i"(__base), "r"(__index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%fs:%a2(%1), %k0 | %k0, fs:[%1+%2] }"
        : "=r"(__value) : "r"(__base), "i"(__index * 4)
        : "memory"
      );
    else
      __asm__ (
        "mov { %%fs:(%1,%2,4), %k0 | %k0, fs:[%1+%2*4] }"
        : "=r"(__value) : "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_ARM64_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "ldr %w0, [x18, %1]"
        : "=r"(__value) : "i"(__base + __index * 4)
        : "memory"
      );
    else
      __asm__ (
        "ldr %w0, [x18, %w1, uxtw 2]"
        : "=r"(__value) : "r"(__base / 4 + __index)
        : "memory"
      );
#elif defined __MCF_M_X8664
  __value = __readgsdword(__base + __index * 4);
#elif defined __MCF_M_X8632
  __value = __readfsdword(__base + __index * 4);
#elif defined __MCF_M_ARM64
  __value = __readx18dword(__base + __index * 4);
#else
#  error unimplemented
#endif
    return __value;
  }

__MCF_TEB_INLINE
void
__MCF_teb_store_32(uint32_t __base, uint32_t __index, uint32_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%gs:%a1 | gs:[%1], %k0 }"
        : : "r"(__value), "i"(__base + __index * 4)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%gs:%a1(,%2,4) | gs:[%1+%2*4], %k0 }"
        : : "r"(__value), "i"(__base), "r"((uint64_t) __index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%gs:%a2(%1) | gs:[%1+%2], %k0 }"
        : : "r"(__value), "r"((uint64_t) __base), "i"(__index * 4)
        : "memory"
      );
    else
      __asm__ volatile (
        "mov { %k0, %%gs:(%1,%2,4) | gs:[%1+%2*4], %k0 }"
        : : "r"(__value), "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_X8632_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%fs:%a1 | fs:[%1], %k0 }"
        : : "r"(__value), "i"(__base + __index * 4)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%fs:%a1(,%2,4) | fs:[%1+%2*4], %k0 }"
        : : "r"(__value), "i"(__base), "r"(__index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%fs:%a2(%1) | fs:[%1+%2], %k0 }"
        : : "r"(__value), "r"(__base), "i"(__index * 4)
        : "memory"
      );
    else
      __asm__ volatile (
        "mov { %k0, %%fs:(%1,%2,4) | fs:[%1+%2*4], %k0 }"
        : : "r"(__value), "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_ARM64_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "str %w0, [x18, %1]"
        : : "r"(__value), "i"(__base + __index * 4)
        : "memory"
      );
    else
      __asm__ volatile (
        "str %w0, [x18, %w1, uxtw 2]"
        : : "r"(__value), "r"(__base / 4 + __index)
        : "memory"
      );
#elif defined __MCF_M_X8664
  __writegsdword(__base + __index * 4, __value);
#elif defined __MCF_M_X8632
  __writefsdword(__base + __index * 4, __value);
#elif defined __MCF_M_ARM64
  __writex18dword(__base + __index * 4, __value);
#else
#  error unimplemented
#endif
  }

__MCF_TEB_INLINE __MCF_FN_PURE
void*
__MCF_teb_load_ptr(uint32_t __base, uint32_t __index)
  __MCF_noexcept
  {
    void* __value;
#if defined __MCF_M_X8664_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%gs:%a1, %q0 | %q0, gs:[%1] }"
        : "=r"(__value) : "i"(__base + __index * 8)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ (
        "mov { %%gs:%a1(,%2,8), %q0 | %q0, gs:[%1+%2*8] }"
        : "=r"(__value) : "i"(__base), "r"((uint64_t) __index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%gs:%a2(%1), %q0 | %q0, gs:[%1+%2] }"
        : "=r"(__value) : "r"((uint64_t) __base), "i"(__index * 8)
        : "memory"
      );
    else
      __asm__ (
        "mov { %%gs:(%1,%2,8), %q0 | %q0, gs:[%1+%2*8] }"
        : "=r"(__value) : "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_X8632_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%fs:%a1, %k0 | %k0, fs:[%1] }"
        : "=r"(__value) : "i"(__base + __index * 4)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ (
        "mov { %%fs:%a1(,%2,4), %k0 | %k0, fs:[%1+%2*4] }"
        : "=r"(__value) : "i"(__base), "r"(__index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "mov { %%fs:%a2(%1), %k0 | %k0, fs:[%1+%2] }"
        : "=r"(__value) : "r"(__base), "i"(__index * 4)
        : "memory"
      );
    else
      __asm__ (
        "mov { %%fs:(%1,%2,4), %k0 | %k0, fs:[%1+%2*4] }"
        : "=r"(__value) : "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_ARM64_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ (
        "ldr %x0, [x18, %1]"
        : "=r"(__value)
        : "i"(__base + __index * 8)
        : "memory"
      );
    else
      __asm__ (
        "ldr %x0, [x18, %w1, uxtw 3]"
        : "=r"(__value)
        : "r"(__base / 8 + __index)
        : "memory"
      );
#elif defined __MCF_M_X8664
  __value = (void*) __readgsqword(__base + __index * 8);
#elif defined __MCF_M_X8632
  __value = (void*) __readfsdword(__base + __index * 4);
#elif defined __MCF_M_ARM64
  __value = (void*) __readx18qword(__base + __index * 8);
#else
#  error unimplemented
#endif
    return __value;
  }

__MCF_TEB_INLINE
void
__MCF_teb_store_ptr(uint32_t __base, uint32_t __index, void* __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %q0, %%gs:%a1 | gs:[%1], %q0 }"
        : : "r"(__value), "i"(__base + __index * 8)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %q0, %%gs:%a1(,%2,8) | gs:[%1+%2*8], %q0 }"
        : : "r"(__value), "i"(__base), "r"((uint64_t) __index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %q0, %%gs:%a2(%1) | gs:[%1+%2], %q0 }"
        : : "r"(__value), "r"((uint64_t) __base), "i"(__index * 8)
        : "memory"
      );
    else
      __asm__ volatile (
        "mov { %q0, %%gs:(%1,%2,8) | gs:[%1+%2*8], %q0 }"
        : : "r"(__value), "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_X8632_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%fs:%a1 | fs:[%1], %k0 }"
        : : "r"(__value), "i"(__base + __index * 4)
        : "memory"
      );
    else if(__builtin_constant_p(__base) && !__builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%fs:%a1(,%2,4) | fs:[%1+%2*4], %k0 }"
        : : "r"(__value), "i"(__base), "r"(__index)
        : "memory"
      );
    else if(!__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "mov { %k0, %%fs:%a2(%1) | fs:[%1+%2], %k0 }"
        : : "r"(__value), "r"(__base), "i"(__index * 4)
        : "memory"
      );
    else
      __asm__ volatile (
        "mov { %k0, %%fs:(%1,%2,4) | fs:[%1+%2*4], %k0 }"
        : : "r"(__value), "r"(__base), "r"(__index)
        : "memory"
      );
#elif defined __MCF_M_ARM64_ASM
    if(__builtin_constant_p(__base) && __builtin_constant_p(__index))
      __asm__ volatile (
        "str %x0, [x18, %1]"
        : : "r"(__value), "i"(__base + __index * 8)
        : "memory"
      );
    else
      __asm__ volatile (
        "str %x0, [x18, %w1, uxtw 3]"
        : : "r"(__value), "r"(__base / 8 + __index)
        : "memory"
      );
#elif defined __MCF_M_X8664
  __writegsqword(__base + __index * 8, (uint64_t) __value);
#elif defined __MCF_M_X8632
  __writefsdword(__base + __index * 4, (uint32_t) __value);
#elif defined __MCF_M_ARM64
  __writex18qword(__base + __index * 8, (uint64_t) __value);
#else
#  error unimplemented
#endif
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TEB_  */
