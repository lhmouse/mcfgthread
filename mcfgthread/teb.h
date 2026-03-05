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

__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_teb(void)
  __MCF_noexcept
  {
#if defined __clang__ && defined __MCF_M_X8664_ASM
    return *(void* __seg_gs*) 0x30;
#elif defined __clang__ && defined __MCF_M_X8632_ASM
    return *(void* __seg_fs*) 0x18;
#elif defined __MCF_M_X8664_ASM
    void* __teb;
    __asm__ ("mov { %%gs:0x30, %0 | %0, gs:[0x30] }" : "=r"(__teb));
    return __teb;
#elif defined __MCF_M_X8632_ASM
    void* __teb;
    __asm__ ("mov { %%fs:0x18, %0 | %0, fs:[0x18] }" : "=r"(__teb));
    return __teb;
#elif defined __MCF_M_ARM64_ASM
    register void* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return __teb;
#elif defined __MCF_M_X8664
    return (void*) __readgsqword(0x30);
#elif defined __MCF_M_X8632
    return (void*) __readfsdword(0x18);
#elif defined __MCF_M_ARM64
    return (void*) __readx18qword(0x30);
#else
#  error unimplemented
#endif
  }

__MCF_TEB_INLINE __MCF_FN_PURE
uint32_t
__MCF_teb_load_32(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __clang__ && defined __MCF_M_X8664_ASM
    return *(uint32_t __seg_gs*) __offset;
#elif defined __clang__ && defined __MCF_M_X8632_ASM
    return *(uint32_t __seg_fs*) __offset;
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8664
    return __readgsdword(__offset);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8632
    return __readfsdword(__offset);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_ARM64
    return __readx18dword(__offset);
#else
    return *((uint32_t*) __MCF_teb() + __offset / 4);
#endif
  }

__MCF_TEB_INLINE
void
__MCF_teb_store_32(uint32_t __offset, uint32_t __value)
  __MCF_noexcept
  {
#if defined __clang__ && defined __MCF_M_X8664_ASM
    *(uint32_t __seg_gs*) __offset = __value;
#elif defined __clang__ && defined __MCF_M_X8632_ASM
    *(uint32_t __seg_fs*) __offset = __value;
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8664
    __writegsdword(__offset, __value);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8632
    __writefsdword(__offset, __value);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_ARM64
    __writex18dword(__offset, __value);
#else
    *((uint32_t*) __MCF_teb() + __offset / 4) = __value;
#endif
  }

__MCF_TEB_INLINE __MCF_FN_PURE
void*
__MCF_teb_load_ptr(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __clang__ && defined __MCF_M_X8664_ASM
    return *(void* __seg_gs*) __offset;
#elif defined __clang__ && defined __MCF_M_X8632_ASM
    return *(void* __seg_fs*) __offset;
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8664
    return (void*) __readgsqword(__offset);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8632
    return (void*) __readfsdword(__offset);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_ARM64
    return (void*) __readx18qword(__offset);
#else
    return *((void**) __MCF_teb() + __offset / __MCF_64_32(8, 4));
#endif
  }

__MCF_TEB_INLINE
void
__MCF_teb_store_ptr(uint32_t __offset, const void* __value)
  __MCF_noexcept
  {
#if defined __clang__ && defined __MCF_M_X8664_ASM
    *(const void* __seg_gs*) __offset = __value;
#elif defined __clang__ && defined __MCF_M_X8632_ASM
    *(const void* __seg_fs*) __offset = __value;
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8664
    __writegsqword(__offset, (uint64_t) __value);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_X8632
    __writefsdword(__offset, (uint32_t) __value);
#elif defined _MSC_VER && !defined __clang__ && defined __MCF_M_ARM64
    __writex18qword(__offset, (uint64_t) __value);
#else
    *((const void**) __MCF_teb() + __offset / __MCF_64_32(8, 4)) = __value;
#endif
  }

__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_peb(void)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
    void* __peb;
    __asm__ ("mov { %%gs:0x60, %0 | %0, gs:[0x60] }" : "=r"(__peb));
    return __peb;
#elif defined __MCF_M_X8632_ASM
    void* __peb;
    __asm__ ("mov { %%fs:0x30, %0 | %0, fs:[0x30] }" : "=r"(__peb));
    return __peb;
#elif defined __MCF_M_ARM64_ASM
    void* __peb;
    __asm__ ("ldr %0, [x18, 0x60]" : "=r"(__peb));
    return __peb;
#elif defined __MCF_M_X8664
    return (void*) __readgsqword(0x60);
#elif defined __MCF_M_X8632
    return (void*) __readfsdword(0x30);
#elif defined __MCF_M_ARM64
    return (void*) __readx18qword(0x60);
#else
#  error unimplemented
#endif
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TEB_  */
