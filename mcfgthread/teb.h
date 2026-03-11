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

/* Load a 32-bit integer at `__offset` of the thread environment block (TEB) of
 * the current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
int32_t
__MCF_teb_load_32(uint32_t __offset)
  __MCF_noexcept
  {
    int32_t __value;
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    __value = *(int32_t __seg_gs*)(uint64_t) __offset;
#  else
    __asm__ ("gs; .insn 0x8B, %k0, %a1"  /* 65 8B := mov R, R/gs:M */
        : "=r"(__value) : "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    __value = *(int32_t __seg_fs*) __offset;
#  else
    __asm__ ("fs; .insn 0x8B, %k0, %a1"  /* 64 8B := mov R, R/fs:M */
        : "=r"(__value) : "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    __value = *(int32_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    __value = (int32_t) __readgsdword(__offset);
#elif defined __MCF_M_X8632
    __value = (int32_t) __readfsdword(__offset);
#elif defined __MCF_M_ARM64
    __value = (int32_t) __readx18dword(__offset);
#else
#  error unimplemented
#endif
    return __value;
  }

/* Stores a 32-bit integer at `__offset` of the thread environment block (TEB)
 * of the current thread. This function may operate on TEB directly and may be
 * more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_32(uint32_t __offset, int32_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int32_t __seg_gs*)(uint64_t) __offset = __value;
#  else
    __asm__ volatile ("gs; .insn 0x89, %k0, %a1"  /* 65 89 := mov R/gs:M, R */
        : : "r"(__value), "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int32_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs; .insn 0x89, %k0, %a1"  /* 64 89 := mov R/fs:M, R */
        : : "r"(__value), "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    *(int32_t*) (__teb + __offset) = __value;
#elif defined __MCF_M_X8664
    __writegsdword(__offset, (uint32_t) __value);
#elif defined __MCF_M_X8632
    __writefsdword(__offset, (uint32_t) __value);
#elif defined __MCF_M_ARM64
    __writex18dword(__offset, (uint32_t) __value);
#else
#  error unimplemented
#endif
  }

/* Load a generic pointer at `__offset` of the thread environment block (TEB)
 * of the current thread. This function may operate on TEB directly and may be
 * more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
void*
__MCF_teb_load_ptr(uint32_t __offset)
  __MCF_noexcept
  {
    void* __value;
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    __value = *(void* __seg_gs*)(uint64_t) __offset;
#  else
    __asm__ ("gs; .insn 0x8B, %0, %a1"  /* 65 8B := mov R, R/gs:M */
        : "=r"(__value) : "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    __value = *(void* __seg_fs*) __offset;
#  else
    __asm__ ("fs; .insn 0x8B, %0, %a1"  /* 64 8B := mov R, R/fs:M */
        : "=r"(__value) : "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    __value = *(void**) (__teb + __offset);
#elif defined __MCF_M_X8664
    __value = (void*) __readgsqword(__offset);
#elif defined __MCF_M_X8632
    __value = (void*) __readfsdword(__offset);
#elif defined __MCF_M_ARM64
    __value = (void*) __readx18qword(__offset);
#else
#  error unimplemented
#endif
    return __value;
  }

/* Stores a generic pointer at `__offset` of the thread environment block (TEB)
 * of the current thread. This function may operate on TEB directly and may be
 * more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_ptr(uint32_t __offset, const void* __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(const void* __seg_gs*)(uint64_t) __offset = __value;
#  else
    __asm__ volatile ("gs; .insn 0x89, %0, %a1"  /* 65 89 := mov R/gs:M, R */
        : : "r"(__value), "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(const void* __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs; .insn 0x89, %0, %a1"  /* 64 89 := mov R/fs:M, R */
        : : "r"(__value), "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    *(const void**) (__teb + __offset) = __value;
#elif defined __MCF_M_X8664
    __writegsqword(__offset, (uint64_t) __value);
#elif defined __MCF_M_X8632
    __writefsdword(__offset, (uint32_t) __value);
#elif defined __MCF_M_ARM64
    __writex18qword(__offset, (uint64_t) __value);
#else
#  error unimplemented
#endif
  }

/* Get a pointer to the process environment block of the current process in the
 * flat address space.  */
__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_peb(void)
  __MCF_noexcept
  {
    return __MCF_teb_load_ptr(__MCF_64_32(0x60, 0x30));
  }

/* Get a pointer to the thread environment block of the current thread in the
 * flat address space.  */
__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_teb(void)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM && defined __FSGSBASE__
    return (void*) __builtin_ia32_rdgsbase64();
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return __teb;
#else
    return __MCF_teb_load_ptr(__MCF_64_32(0x30, 0x18));
#endif
  }

/* Get the ID of the current process. This is the same value as
 * `GetCurrentProcessId()`, but as a signed integer.  */
__MCF_TEB_INLINE __MCF_FN_CONST
int32_t
__MCF_pid(void)
  __MCF_noexcept
  {
    return __MCF_teb_load_32(__MCF_64_32(0x40, 0x20));
  }

/* Get the ID of the current thread. This is the same value as
 * `GetCurrentThreadId()`, but as a signed integer.  */
__MCF_TEB_INLINE __MCF_FN_CONST
int32_t
__MCF_tid(void)
  __MCF_noexcept
  {
    return __MCF_teb_load_32(__MCF_64_32(0x48, 0x24));
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TEB_  */
