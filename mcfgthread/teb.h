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

/** Loads an 8-bit integer at `offset` of the thread environment block (TEB) of
 * the current thread.
 *
 * @param `offset` specifies the address of the integer to load.
 * @returns the integer at the specified offset as a signed integer.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_PURE
int8_t
__MCF_teb_load_8(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(int8_t __seg_gs*) (__offset + 0ULL);
#  else
    int8_t __value;
    __asm__ ("gs { movzbl %a1, %k0 | movzx %k0, BYTE PTR %a1 }"
             : "=q"(__value) : "Ts"(__offset + 0ULL)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(int8_t __seg_fs*) __offset;
#  else
    int8_t __value;
    __asm__ ("fs { movzbl %a1, %k0 | movzx %k0, BYTE PTR %a1 }"
             : "=q"(__value) : "Ts"(__offset)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return *(int8_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    return (int8_t) __readgsbyte(__offset);
#elif defined __MCF_M_X8632
    return (int8_t) __readfsbyte(__offset);
#elif defined __MCF_M_ARM64
    return (int8_t) __readx18byte(__offset);
#else
#  error unimplemented
#endif
  }

/** Stores an 8-bit integer at `offset` of the thread environment block (TEB) of
 * the current thread.
 *
 * @param `offset` specifies the address of the integer to store.
 * @param `value` is the integer to store at the specified offset.
 * @returns nothing.
 * @since 2.4  */
__MCF_TEB_INLINE
void
__MCF_teb_store_8(uint32_t __offset, int8_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int8_t __seg_gs*) (__offset + 0ULL) = __value;
#  else
    __asm__ volatile ("gs { movb %1, %a0 | mov BYTE PTR %a0, %1 }"
                      : : "Ts"(__offset + 0ULL), "qi"(__value)
                      : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int8_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs { movb %1, %a0 | mov BYTE PTR %a0, %1 }"
                      : : "Ts"(__offset), "ri"(__value)
                      : "memory");
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

/** Loads a 16-bit integer at `offset` of the thread environment block (TEB) of
 * the current thread.
 *
 * @param `offset` specifies the address of the integer to load.
 * @returns the integer at the specified offset as a signed integer.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_PURE
int16_t
__MCF_teb_load_16(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(int16_t __seg_gs*) (__offset + 0ULL);
#  else
    int16_t __value;
    __asm__ ("gs { movzwl %a1, %k0 | movzx %k0, WORD PTR %a1 }"
             : "=r"(__value) : "Ts"(__offset + 0ULL)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(int16_t __seg_fs*) __offset;
#  else
    int16_t __value;
    __asm__ ("fs { movzwl %a1, %k0 | movzx %k0, WORD PTR %a1 }"
             : "=r"(__value) : "Ts"(__offset)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return *(int16_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    return (int16_t) __readgsword(__offset);
#elif defined __MCF_M_X8632
    return (int16_t) __readfsword(__offset);
#elif defined __MCF_M_ARM64
    return (int16_t) __readx18word(__offset);
#else
#  error unimplemented
#endif
  }

/** Stores a 16-bit integer at `offset` of the thread environment block (TEB) of
 * the current thread.
 *
 * @param `offset` specifies the address of the integer to store.
 * @param `value` is the integer to store at the specified offset.
 * @returns nothing.
 * @since 2.4  */
__MCF_TEB_INLINE
void
__MCF_teb_store_16(uint32_t __offset, int16_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int16_t __seg_gs*) (__offset + 0ULL) = __value;
#  else
    __asm__ volatile ("gs { movw %1, %a0 | mov WORD PTR %a0, %1 }"
                      : : "Ts"(__offset + 0ULL), "ri"(__value)
                      : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int16_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs { movw %1, %a0 | mov WORD PTR %a0, %1 }"
                      : : "Ts"(__offset), "ri"(__value)
                      : "memory");
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

/** Loads a 32-bit integer at `offset` of the thread environment block (TEB) of
 * the current thread.
 *
 * @param `offset` specifies the address of the integer to load.
 * @returns the integer at the specified offset as a signed integer.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_PURE
int32_t
__MCF_teb_load_32(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(int32_t __seg_gs*) (__offset + 0ULL);
#  else
    int32_t __value;
    __asm__ ("gs { movl %a1, %k0 | mov %k0, DWORD PTR %a1 }"
             : "=r"(__value) : "Ts"(__offset + 0ULL)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(int32_t __seg_fs*) __offset;
#  else
    int32_t __value;
    __asm__ ("fs { movl %a1, %k0 | mov %k0, DWORD PTR %a1 }"
             : "=r"(__value) : "Ts"(__offset)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return *(int32_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    return (int32_t) __readgsdword(__offset);
#elif defined __MCF_M_X8632
    return (int32_t) __readfsdword(__offset);
#elif defined __MCF_M_ARM64
    return (int32_t) __readx18dword(__offset);
#else
#  error unimplemented
#endif
  }

/** Stores a 32-bit integer at `offset` of the thread environment block (TEB) of
 * the current thread.
 *
 * @param `offset` specifies the address of the integer to store.
 * @param `value` is the integer to store at the specified offset.
 * @returns nothing.
 * @since 2.4  */
__MCF_TEB_INLINE
void
__MCF_teb_store_32(uint32_t __offset, int32_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int32_t __seg_gs*) (__offset + 0ULL) = __value;
#  else
    __asm__ volatile ("gs { movl %1, %a0 | mov DWORD PTR %a0, %1 }"
                      : : "Ts"(__offset + 0ULL), "ri"(__value)
                      : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int32_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs { movl %1, %a0 | mov DWORD PTR %a0, %1 }"
                      : : "Ts"(__offset), "ri"(__value)
                      : "memory");
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

/** Loads a pointer-size integer at `offset` of the thread environment block (TEB)
 * of the current thread.
 *
 * @param `offset` specifies the address of the integer to load.
 * @returns the integer at the specified offset as a signed integer.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_PURE
intptr_t
__MCF_teb_load_ptr(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(int64_t __seg_gs*) (__offset + 0ULL);
#  else
    int64_t __value;
    __asm__ ("gs { movq %a1, %q0 | mov %q0, QWORD PTR %a1 }"
             : "=r"(__value) : "Ts"(__offset + 0ULL)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(int32_t __seg_fs*) __offset;
#  else
    int32_t __value;
    __asm__ ("fs { movl %a1, %k0 | mov %k0, DWORD PTR %a1 }"
             : "=r"(__value) : "Ts"(__offset)
             : "memory");
    return __value;
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return *(int64_t*) (__teb + __offset);
#elif defined __MCF_M_X8664
    return (int64_t) __readgsqword(__offset);
#elif defined __MCF_M_X8632
    return (int32_t) __readfsdword(__offset);
#elif defined __MCF_M_ARM64
    return (int64_t) __readx18qword(__offset);
#else
#  error unimplemented
#endif
  }

/** Stores a pointer-size integer at `offset` of the thread environment block
 * (TEB) of the current thread.
 *
 * @param `offset` specifies the address of the integer to store.
 * @param `value` is the integer to store at the specified offset.
 * @returns nothing.
 * @since 2.4  */
__MCF_TEB_INLINE
void
__MCF_teb_store_ptr(uint32_t __offset, intptr_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    *(int64_t __seg_gs*) (__offset + 0ULL) = __value;
#  else
    __asm__ volatile ("gs { movq %1, %a0 | mov QWORD PTR %a0, %1 }"
                      : : "Ts"(__offset + 0ULL), "re"(__value)
                      : "memory");
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    *(int32_t __seg_fs*) __offset = __value;
#  else
    __asm__ volatile ("fs { movl %1, %a0 | mov DWORD PTR %a0, %1 }"
                      : : "Ts"(__offset), "ri"(__value)
                      : "memory");
#  endif
#elif defined __MCF_M_ARM64_ASM
    register char* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    *(int64_t*) (__teb + __offset) = __value;
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

/** Gets the process environment block (PEB) of the current process.
 *
 * @returns a pointer to the PEB in flat address space.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_peb(void)
  __MCF_noexcept
  {
    /* `TePeb`; WDK: ksamd64.inc, ksarm64.h, ks386.inc  */
    return (void*) __MCF_teb_load_ptr(__MCF_64_32(0x60, 0x30));
  }

/** Gets the thread environment block (TEB) of the current thread.
 *
 * @returns a pointer to the TEB in flat address space.
 * @since 2.4  */
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
    /* `TeSelf`; WDK: ksamd64.inc, ksarm64.h, ks386.inc  */
    return (void*) __MCF_teb_load_ptr(__MCF_64_32(0x30, 0x18));
#endif
  }

/** Gets the identifier of the current process.
 *
 * This is the same value that is returned by `GetCurrentProcessId()`, but as
 * a signed integer.
 *
 * @returns the identifier of the current process as a signed integer.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_CONST
int32_t
__MCF_pid(void)
  __MCF_noexcept
  {
    /* `TeClientId`; WDK: ksamd64.inc, ksarm64.h, ks386.inc  */
    return __MCF_teb_load_32(__MCF_64_32(0x40, 0x20));
  }

/** Gets the identifier of the current thread.
 *
 * This is the same value that is returned by `GetCurrentThreadId()`, but as
 * a signed integer.
 *
 * @returns the identifier of the current thread as a signed integer.
 * @since 2.4  */
__MCF_TEB_INLINE __MCF_FN_CONST
int32_t
__MCF_tid(void)
  __MCF_noexcept
  {
    /* `TeClientId`; WDK: ksamd64.inc, ksarm64.h, ks386.inc  */
    return __MCF_teb_load_32(__MCF_64_32(0x48, 0x24));
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TEB_  */
