/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
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

/* Load an 8-bit integer at `__offset` of the thread environment block (TEB) of
 * the current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
int8_t
__MCF_teb_load_8(uint32_t __offset)
  __MCF_noexcept
  {
    int8_t __value;
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    __value = *(int8_t __seg_gs*)(uint64_t) __offset;
#  else
    __asm__ ("gs; .insn 0x0FB6, %k0, %a1"  /* 0FB6 := movzx R32, R/M8 */
        : "=q"(__value) : "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    __value = *(int8_t __seg_fs*) __offset;
#  else
    __asm__ ("fs; .insn 0x0FB6, %k0, %a1"  /* 0FB6 := movzx R32, R/M8 */
        : "=q"(__value) : "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    __value = *(int8_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    __value = (int8_t) __readgsbyte(__offset);
#elif defined __MCF_M_X8632
    __value = (int8_t) __readfsbyte(__offset);
#elif defined __MCF_M_ARM64
    __value = (int8_t) __readx18byte(__offset);
#else
#  error unimplemented
#endif
    return __value;
  }

/* Stores an 8-bit integer at `__offset` of the thread environment block (TEB)
 * of the current thread. This function may operate on TEB directly and may be
 * more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_8(uint32_t __offset, int8_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int8_t __seg_gs*)(uint64_t) __offset = __value;
#  else
    __asm__ volatile ("gs; .insn 0x88, %0, %a1"  /* 88 := mov R/M8, R8 */
        : : "q"(__value), "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int8_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs; .insn 0x88, %0, %a1"  /* 88 := mov R/M8, R8 */
        : : "r"(__value), "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    *(int8_t*) (__teb + __offset) = __value;
#elif defined __MCF_M_X8664
    __writegsbyte(__offset, (uint8_t) __value);
#elif defined __MCF_M_X8632
    __writefsbyte(__offset, (uint8_t) __value);
#elif defined __MCF_M_ARM64
    __writex18byte(__offset, (uint8_t) __value);
#else
#  error unimplemented
#endif
  }

/* Load a 16-bit integer at `__offset` of the thread environment block (TEB) of
 * the current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
int16_t
__MCF_teb_load_16(uint32_t __offset)
  __MCF_noexcept
  {
    int16_t __value;
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    __value = *(int16_t __seg_gs*)(uint64_t) __offset;
#  else
    __asm__ ("gs; .insn 0x0FB7, %k0, %a1"  /* 0FB7 := movzx R32, R/M16 */
        : "=r"(__value) : "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    __value = *(int16_t __seg_fs*) __offset;
#  else
    __asm__ ("fs; .insn 0x0FB7, %k0, %a1"  /* 0FB7 := movzx R32, R/M16 */
        : "=r"(__value) : "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    __value = *(int16_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    __value = (int16_t) __readgsword(__offset);
#elif defined __MCF_M_X8632
    __value = (int16_t) __readfsword(__offset);
#elif defined __MCF_M_ARM64
    __value = (int16_t) __readx18word(__offset);
#else
#  error unimplemented
#endif
    return __value;
  }

/* Stores a 16-bit integer at `__offset` of the thread environment block (TEB)
 * of the current thread. This function may operate on TEB directly and may be
 * more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_16(uint32_t __offset, int16_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int16_t __seg_gs*)(uint64_t) __offset = __value;
#  else
    __asm__ volatile ("gs; .insn 0x89, %0, %a1"  /* 66:89 := mov R/M16, R16 */
        : : "r"(__value), "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int16_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs; .insn 0x89, %0, %a1"  /* 66:89 := mov R/M16, R16 */
        : : "r"(__value), "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    *(int16_t*) (__teb + __offset) = __value;
#elif defined __MCF_M_X8664
    __writegsword(__offset, (uint16_t) __value);
#elif defined __MCF_M_X8632
    __writefsword(__offset, (uint16_t) __value);
#elif defined __MCF_M_ARM64
    __writex18word(__offset, (uint16_t) __value);
#else
#  error unimplemented
#endif
  }

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
    __asm__ ("gs; .insn 0x8B, %0, %a1"  /* 8B := mov R32, R/M32 */
        : "=r"(__value) : "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    __value = *(int32_t __seg_fs*) __offset;
#  else
    __asm__ ("fs; .insn 0x8B, %0, %a1"  /* 8B := mov R32, R/M32 */
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
    __asm__ volatile ("gs; .insn 0x89, %0, %a1"  /* 89 := mov R/M32, R32 */
        : : "r"(__value), "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int32_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs; .insn 0x89, %0, %a1"  /* 89 := mov R/M32, R32 */
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

/* Load a pointer-size integer at `__offset` of the thread environment block
 * (TEB) of the current thread. This function may operate on TEB directly and
 * may be more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
intptr_t
__MCF_teb_load_ptr(uint32_t __offset)
  __MCF_noexcept
  {
    intptr_t __value;
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    __value = *(intptr_t __seg_gs*)(uint64_t) __offset;
#  else
    __asm__ ("gs; .insn 0x8B, %0, %a1"  /* REX.W:8B := mov R64, R/M64 */
        : "=r"(__value) : "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    __value = *(intptr_t __seg_fs*) __offset;
#  else
    __asm__ ("fs; .insn 0x8B, %0, %a1"  /* 8B := mov R32, R/M32 */
        : "=r"(__value) : "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    __value = *(intptr_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    __value = (intptr_t) __readgsqword(__offset);
#elif defined __MCF_M_X8632
    __value = (intptr_t) __readfsdword(__offset);
#elif defined __MCF_M_ARM64
    __value = (intptr_t) __readx18qword(__offset);
#else
#  error unimplemented
#endif
    return __value;
  }

/* Stores a pointer-size integer at `__offset` of the thread environment block
 * (TEB) of the current thread. This function may operate on TEB directly and
 * may be more efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_ptr(uint32_t __offset, intptr_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(intptr_t __seg_gs*)(uint64_t) __offset = __value;
#  else
    __asm__ volatile ("gs; .insn 0x89, %0, %a1"  /* REX.W:89 := mov R/M64, R64 */
        : : "r"(__value), "Ts"((uint64_t) __offset) : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(intptr_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs; .insn 0x89, %0, %a1"  /* 89 := mov R/M32, R32 */
        : : "r"(__value), "Ts"(__offset) : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    *(intptr_t*) (__teb + __offset) = __value;
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
    return (void*) __MCF_teb_load_ptr(__MCF_64_32(0x60, 0x30));
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
    return (void*) __MCF_teb_load_ptr(__MCF_64_32(0x30, 0x18));
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
