/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_FWD_
#define __MCFGTHREAD_FWD_

#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <intrin.h>

#if !defined _WIN32_WINNT || (_WIN32_WINNT < 0x0601)
#  error Please define `_WIN32_WINNT` to at least Windows 7.
#endif

#define __MCF_C(...)     __VA_ARGS__
#define __MCF_CXX(...)
#define __MCF_noexcept
#define __MCF_nullptr    __MCF_IPTR_0

#if defined __cplusplus
#  undef __MCF_CXX
#  define __MCF_CXX(...)   __VA_ARGS__
#  undef __MCF_C
#  define __MCF_C(...)
#  undef __MCF_noexcept
#  define __MCF_noexcept   throw()
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_FWD_IMPORT
#  define __MCF_FWD_IMPORT
#  define __MCF_FWD_INLINE  __MCF_GNU_INLINE
#endif

/* Define language-support macros. We start from C89 (which doesn't have an
 * identification macro) and then redefine these macros according to
 * `__STDC_VERSION__` and `__cplusplus`.  */
#define __MCF_C99(...)
#define __MCF_C11(...)
#define __MCF_C17(...)
#define __MCF_C23(...)

#define __MCF_CXX11(...)
#define __MCF_CXX14(...)
#define __MCF_CXX17(...)
#define __MCF_CXX20(...)
#define __MCF_CXX23(...)

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 199901L)
#  undef __MCF_C99
#  define __MCF_C99(...)   __VA_ARGS__
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 201112L)
#  undef __MCF_C11
#  define __MCF_C11(...)   __VA_ARGS__
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 201710L)
#  undef __MCF_C17
#  define __MCF_C17(...)   __VA_ARGS__
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 202311L)
#  undef __MCF_C23
#  define __MCF_C23(...)   __VA_ARGS__
#  undef __MCF_nullptr
#  define __MCF_nullptr   nullptr
#endif

#if defined __cplusplus && (defined _MSC_VER || (__cplusplus >= 201103L))
#  undef __MCF_CXX11
#  define __MCF_CXX11(...)   __VA_ARGS__
#  undef __MCF_noexcept
#  define __MCF_noexcept   noexcept
#  undef __MCF_nullptr
#  define __MCF_nullptr    nullptr
#endif

#if defined __cplusplus && (defined _MSC_VER || (__cplusplus >= 201402L))
#  undef __MCF_CXX14
#  define __MCF_CXX14(...)   __VA_ARGS__
#endif

#if defined __cplusplus && (__cplusplus >= 201703L)
#  undef __MCF_CXX17
#  define __MCF_CXX17(...)   __VA_ARGS__
#endif

#if defined __cplusplus && (__cplusplus >= 202002L)
#  undef __MCF_CXX20
#  define __MCF_CXX20(...)   __VA_ARGS__
#endif

#if defined __cplusplus && (__cplusplus >= 202302L)
#  undef __MCF_CXX23
#  define __MCF_CXX23(...)   __VA_ARGS__
#endif

/* Define compiler-specific stuff. In the case of clang-cl, prefer GNU
 * extensions to Microsoft ones.  */
#if defined __GNUC__ || defined __clang__
#  define __MCF_EX             __extension__
#  define __MCF_GNU_INLINE      extern __inline__ __attribute__((__gnu_inline__))
#  define __MCF_ALWAYS_INLINE   extern __inline__ __attribute__((__gnu_inline__, __always_inline__))
#  define __MCF_NEVER_INLINE   __attribute__((__noinline__))
#  define __MCF_NEVER_RETURN   __attribute__((__noreturn__))
#  define __MCF_FN_CONST       __attribute__((__const__))
#  define __MCF_FN_PURE       __attribute__((__pure__))
#  define __MCF_FN_COLD       __attribute__((__cold__))
#  define __MCF_ASM_CALL(x)   __asm__(__MCF_S(__USER_LABEL_PREFIX__) #x)
#  define __MCF_ALIGNED(x)    __attribute__((__aligned__(x)))
#  define __MCF_UNREACHABLE   __builtin_unreachable()
#else
#  define __MCF_EX             /* unsupported */
#  define __MCF_GNU_INLINE      __inline
#  define __MCF_ALWAYS_INLINE   __forceinline
#  define __MCF_NEVER_INLINE   __declspec(noinline)
#  define __MCF_NEVER_RETURN   __declspec(noreturn)
#  define __MCF_FN_CONST       __declspec(noalias)
#  define __MCF_FN_PURE       __declspec(noalias)
#  define __MCF_FN_COLD       /* unsupported */
#  define __MCF_ASM_CALL(x)   /* unnecessary */
#  define __MCF_ALIGNED(x)    __declspec(align(x))
#  define __MCF_UNREACHABLE   __assume(0)
#endif

#if defined _MSC_VER && defined __cplusplus
#  define __MCF_MAY_THROW   throw(...)
#else
#  define __MCF_MAY_THROW
#endif

/* Define how the TEB is accessed. Due to technical limitations, the base offset
 * must be a constant multiple of operand size, and there are different macros
 * depending on whether the address is immediate.  */
#if (defined __GNUC__ || defined __clang__) && (defined __amd64__ && !defined __arm64ec__)

#  define __MCF_TEB_LOAD_32_IMMEDIATE(out, offset)  \
      __asm__ volatile (  \
         "mov { %%gs:%c1, %k0 | %k0, gs:[%c1] }"  \
          : "=r"(*(out)) : "e"(offset))

#  define __MCF_TEB_STORE_32_IMMEDIATE(offset, value)  \
      __asm__ volatile (  \
         "mov { %k1, %%gs:%c0 | gs:[%c0], %k1 }"  \
          : : "e"(offset), "r"(value))

#  define __MCF_TEB_LOAD_32_INDEXED(out, offset, index)  \
      __asm__ volatile (  \
         "mov { %%gs:%c1(,%q2,4), %k0 | %k0, gs:[%c1+%q2*4] }"  \
          : "=r"(*(out)) : "e"(offset), "r"(index))

#  define __MCF_TEB_STORE_32_INDEXED(offset, index, value)  \
      __asm__ volatile (  \
         "mov { %k2, %%gs:%c0(,%q1,4) | gs:[%c0+%q1*4], %k2 }"  \
          : : "e"(offset), "r"(index), "r"(value))

#  define __MCF_TEB_LOAD_PTR_IMMEDIATE(out, offset)  \
      __asm__ volatile (  \
         "mov { %%gs:%c1, %0 | %0, gs:[%c1] }"  \
          : "=r"(*(out)) : "e"(offset))

#  define __MCF_TEB_STORE_PTR_IMMEDIATE(offset, value)  \
      __asm__ volatile (  \
         "mov { %1, %%gs:%c0 | gs:[%c0], %1 }"  \
          : : "e"(offset), "r"(value))

#  define __MCF_TEB_LOAD_PTR_INDEXED(out, offset, index)  \
      __asm__ volatile (  \
         "mov { %%gs:%c1(,%q2,8), %0 | %0, gs:[%c1+%q2*8] }"  \
          : "=r"(*(out)) : "e"(offset), "r"(index))

#  define __MCF_TEB_STORE_PTR_INDEXED(offset, index, value)  \
      __asm__ volatile (  \
         "mov { %2, %%gs:%c0(,%q1,8) | gs:[%c0+%q1*8], %2 }"  \
          : : "e"(offset), "r"(index), "r"(value))

#  define __MCF_64_32(x, y)  x

#elif defined _MSC_VER && (defined _M_X64 && !defined _M_ARM64EC)

#  define __MCF_TEB_LOAD_32_IMMEDIATE(out, offset)  \
      (*(out) = __readgsdword((offset)))

#  define __MCF_TEB_STORE_32_IMMEDIATE(offset, value)  \
      __writegsdword((offset), (value))

#  define __MCF_TEB_LOAD_32_INDEXED(out, offset, index)  \
      (*(out) = __readgsdword((offset) + (index) * 4U))

#  define __MCF_TEB_STORE_32_INDEXED(offset, index, value)  \
      __writegsdword((offset) + (index) * 4U, (value))

#  define __MCF_TEB_LOAD_PTR_IMMEDIATE(out, offset)  \
      (*(out) = __readgsqword((offset)))

#  define __MCF_TEB_STORE_PTR_IMMEDIATE(offset, value)  \
      __writegsqword((offset), (value))

#  define __MCF_TEB_LOAD_PTR_INDEXED(out, offset, index)  \
      (*(out) = __readgsqword((offset) + (index) * 8U))

#  define __MCF_TEB_STORE_PTR_INDEXED(offset, index, value)  \
      __writegsqword((offset) + (index) * 8U, (value))

#  define __MCF_64_32(x, y)  x

#elif (defined __GNUC__ || defined __clang__) && defined __i386__

#  define __MCF_TEB_LOAD_32_IMMEDIATE(out, offset)  \
      __asm__ volatile (  \
         "mov { %%fs:%c1, %k0 | %k0, fs:[%c1] }"  \
          : "=r"(*(out)) : "e"(offset))

#  define __MCF_TEB_STORE_32_IMMEDIATE(offset, value)  \
      __asm__ volatile (  \
         "mov { %k1, %%fs:%c0 | fs:[%c0], %k1 }"  \
          : : "e"(offset), "r"(value))

#  define __MCF_TEB_LOAD_32_INDEXED(out, offset, index)  \
      __asm__ volatile (  \
         "mov { %%fs:%c1(,%k2,4), %k0 | %k0, fs:[%c1+%k2*4] }"  \
          : "=r"(*(out)) : "e"(offset), "r"(index))

#  define __MCF_TEB_STORE_32_INDEXED(offset, index, value)  \
      __asm__ volatile (  \
         "mov { %k1, %%fs:%c0(,%k1,4) | fs:[%c0+%k1*4], %k2 }"  \
          : : "e"(offset), "r"(index), "r"(value))

#  define __MCF_TEB_LOAD_PTR_IMMEDIATE(out, offset)  \
      __asm__ volatile (  \
         "mov { %%fs:%c1, %0 | %0, fs:[%c1] }"  \
          : "=r"(*(out)) : "e"(offset))

#  define __MCF_TEB_STORE_PTR_IMMEDIATE(offset, value)  \
      __asm__ volatile (  \
         "mov { %1, %%fs:%c0 | fs:[%c0], %1 }"  \
          : : "e"(offset), "r"(value))

#  define __MCF_TEB_LOAD_PTR_INDEXED(out, offset, index)  \
      __asm__ volatile (  \
         "mov { %%fs:%c1(,%k2,4), %0 | %0, fs:[%c1+%k2*4] }"  \
          : "=r"(*(out)) : "e"(offset), "r"(index))

#  define __MCF_TEB_STORE_PTR_INDEXED(offset, index, value)  \
      __asm__ volatile (  \
         "mov { %2, %%fs:%c0(,%k1,4) | fs:[%c0+%k1*4], %2 }"  \
          : : "e"(offset), "r"(index), "r"(value))

#  define __MCF_64_32(x, y)  y

#elif defined _MSC_VER && defined _M_IX86

#  define __MCF_TEB_LOAD_32_IMMEDIATE(out, offset)  \
      (*(out) = __readfsdword((offset)))

#  define __MCF_TEB_STORE_32_IMMEDIATE(offset, value)  \
      __writefsdword((offset), (value))

#  define __MCF_TEB_LOAD_32_INDEXED(out, offset, index)  \
      *(out) = __readfsdword((offset) + (index) * 4U)

#  define __MCF_TEB_STORE_32_INDEXED(offset, index, value)  \
      __writefsdword((offset) + (index) * 4U, (value))

#  define __MCF_TEB_LOAD_PTR_IMMEDIATE(out, offset)  \
      (*(out) = __readfsdword((offset)))

#  define __MCF_TEB_STORE_PTR_IMMEDIATE(offset, value)  \
      __writefsdword((offset), (value))

#  define __MCF_TEB_LOAD_PTR_INDEXED(out, offset, index)  \
      (*(out) = __readfsdword((offset) + (index) * 4U))

#  define __MCF_TEB_STORE_PTR_INDEXED(offset, index, value)  \
      __writefsdword((offset) + (index) * 4U, (value))

#  define __MCF_64_32(x, y)  y

#elif (defined __GNUC__ || defined __clang__) && (defined __aarch64__ || defined __arm64ec__)

#  define __MCF_TEB_LOAD_32_IMMEDIATE(out, offset)  \
      __asm__ volatile (  \
         "ldr %w0, [x18, %c1]"  \
          : "=r"(*(out)) : "M"(offset))

#  define __MCF_TEB_STORE_32_IMMEDIATE(offset, value)  \
      __asm__ volatile (  \
         "str %w0, [x18, %c1]"  \
          : : "r"(value), "M"(offset))

#  define __MCF_TEB_LOAD_32_INDEXED(out, offset, index)  \
      __asm__ volatile (  \
         "ldr %w0, [x18, %x1, lsl 2]"  \
          : "=r"(*(out)) : "r"((offset) / 4U + (index)))

#  define __MCF_TEB_STORE_32_INDEXED(offset, index, value)  \
      __asm__ volatile (  \
         "str %w0, [x18, %x1, lsl 2]"  \
          : : "r"(value), "r"((offset) / 4U + (index)))

#  define __MCF_TEB_LOAD_PTR_IMMEDIATE(out, offset)  \
      __asm__ volatile (  \
         "ldr %0, [x18, %c1]"  \
          : "=r"(*(out)) : "M"(offset))

#  define __MCF_TEB_STORE_PTR_IMMEDIATE(offset, value)  \
      __asm__ volatile (  \
         "str %0, [x18, %c1]"  \
          : : "r"(value), "M"(offset))

#  define __MCF_TEB_LOAD_PTR_INDEXED(out, offset, index)  \
      __asm__ volatile (  \
         "ldr %0, [x18, %x1, lsl 3]"  \
          : "=r"(*(out)) : "r"((offset) / 8U + (index)))

#  define __MCF_TEB_STORE_PTR_INDEXED(offset, index, value)  \
      __asm__ volatile (  \
         "str %0, [x18, %x1, lsl 3]"  \
          : : "r"(value), "r"((offset) / 8U + (index)))

#  define __MCF_64_32(x, y)  x

#elif defined _MSC_VER && (defined _M_ARM64 || defined _M_ARM64EC)

#  define __MCF_TEB_LOAD_32_IMMEDIATE(out, offset)  \
      (*(out) = __readx18dword((offset)))

#  define __MCF_TEB_STORE_32_IMMEDIATE(offset, value)  \
      __writex18dword((offset), (value))

#  define __MCF_TEB_LOAD_32_INDEXED(out, offset, index)  \
      (*(out) = __readx18dword((offset) + (index) * 4U))

#  define __MCF_TEB_STORE_32_INDEXED(offset, index, value)  \
      __writex18dword((offset) + (index) * 4U, (value))

#  define __MCF_TEB_LOAD_PTR_IMMEDIATE(out, offset)  \
      (*(out) = __readx18qword((offset)))

#  define __MCF_TEB_STORE_PTR_IMMEDIATE(offset, value)  \
      __writex18qword((offset), (value))

#  define __MCF_TEB_LOAD_PTR_INDEXED(out, offset, index)  \
      (*(out) = __readx18qword((offset) + (index) * 8U))

#  define __MCF_TEB_STORE_PTR_INDEXED(offset, index, value)  \
      __writex18qword((offset) + (index) * 8U, (value))

#  define __MCF_64_32(x, y)  x

#endif  /* compiler and target  */

#define __MCF_S_(...)     #__VA_ARGS__
#define __MCF_S(...)       __MCF_S_(__VA_ARGS__)
#define __MCF_C2_(x, y)     x##y
#define __MCF_C2(x, y)       __MCF_C2_(x, y)
#define __MCF_C3_(x, y, z)   x##y##z
#define __MCF_C3(x, y, z)    __MCF_C3_(x, y, z)
#define __MCF_0_INIT           { __MCF_C(0) }
#define __MCF_SET_IF(x, ...)    ((void) ((x) && (*(x) = (__VA_ARGS__))))

/* These are necessary when the header is compiled as C89 or C++98.  */
#define __MCF_PTR_BITS    __MCF_64_32(64, 32)
#define __MCF_IPTR_MIN    __MCF_64_32(LLONG_MIN, INT_MIN)
#define __MCF_IPTR_0      __MCF_64_32(0LL, 0)
#define __MCF_IPTR_MAX    __MCF_64_32(LLONG_MAX, INT_MAX)
#define __MCF_UPTR_0      __MCF_64_32(0ULL, 0U)
#define __MCF_UPTR_MAX    __MCF_64_32(ULLONG_MAX, UINT_MAX)

/* For debug builds, `__MCF_UNREACHABLE` shall effect a breakpoint.  */
__MCF_NEVER_RETURN
void
__MCF__Exit(int __status) __MCF_noexcept;

#if defined __MCF_DEBUG
#  undef __MCF_UNREACHABLE
#  define __MCF_UNREACHABLE   (__debugbreak(), __MCF__Exit(668))
#endif

/* Some compilers warn about casts between pointers, so launder the pointer via
 * an in-between integral type.  */
#ifdef __cplusplus
#  define __MCF_CAST_PTR(T, ...)   (reinterpret_cast<T*>(reinterpret_cast< ::intptr_t>(__VA_ARGS__)))
#else
#  define __MCF_CAST_PTR(T, ...)   ((T*)(intptr_t) (__VA_ARGS__))
#endif

/* The `__MCF_STATIC_ASSERT_0()` macro is an expression that yields zero if it
 * compiles anyway. Its argument must be a constant expression.  */
#ifdef __cplusplus
extern "C++" {
template<bool> struct __MCF_static_assert;
template<> struct __MCF_static_assert<true> { };
}  /* extern "C++"  */
#  define __MCF_STATIC_ASSERT_T(...)   ::__MCF_static_assert<bool(__VA_ARGS__)>
#else
#  define __MCF_STATIC_ASSERT_T(...)   struct { int: 1|-!(__VA_ARGS__); }
#endif

#define __MCF_STATIC_ASSERT_0(...)   (0 & (int) sizeof(__MCF_STATIC_ASSERT_T(__VA_ARGS__)))
#define __MCF_STATIC_ASSERT(...)    ((void) sizeof(__MCF_STATIC_ASSERT_T(__VA_ARGS__)))

/* The `__MCF_ASSERT()` and `__MCF_CHECK()` macros perform run-time checks. If
 * an argument yields false, `__MCF_ASSERT()` results in undefined behavior,
 * and `__MCF_CHECK()` effects abnormal termination of the current program.  */
#define __MCF_ASSERT(...)    ((__VA_ARGS__) ? (void) 0 : __MCF_UNREACHABLE)
#define __MCF_CHECK(...)    ((__VA_ARGS__) ? (void) 0 : __MCF_runtime_failure(__MCF_EX __func__))

typedef struct __MCF_dtor_element __MCF_dtor_element;
typedef struct __MCF_dtor_queue __MCF_dtor_queue;
typedef struct __MCF_tls_table __MCF_tls_table;
typedef struct __MCF_tls_element __MCF_tls_element;

typedef struct __MCF_cond _MCF_cond;
typedef struct __MCF_mutex _MCF_mutex;
typedef struct __MCF_shared_mutex _MCF_shared_mutex;
typedef struct __MCF_once _MCF_once;
typedef struct __MCF_sem _MCF_sem;
typedef struct __MCF_event _MCF_event;
typedef struct __MCF_thread _MCF_thread;
typedef int _MCF_thread_priority;
typedef struct __MCF_tls_key _MCF_tls_key;

typedef struct timespec __MCF_timespec;
typedef void* __MCF_HANDLE;

/* See `_MCF_cond_wait()` for details about these callbacks.  */
typedef intptr_t _MCF_cond_unlock_callback(intptr_t __lock_arg);
typedef void _MCF_cond_relock_callback(intptr_t __lock_arg, intptr_t __unlocked);

/* Define the prototype for thread procedures.  */
typedef void _MCF_thread_procedure(_MCF_thread* __thrd);

/* Define the prototype for destructors for `_MCF_tls_key_new()`.  */
typedef void _MCF_tls_dtor(void* __ptr);

/* Define the prototype for `atexit()` and `at_quick_exit()`.  */
typedef void __MCF_atexit_callback(void);

/* Define the prototype for `call_once()`.  */
typedef void __MCF_once_callback(void);

/* Note: In the case of i386, the argument is passed both via the ECX register
 * and on the stack, to allow both `__cdecl` and `__thiscall` functions to work
 * properly. The function prototype is declared for compatibility with GCC.  */
typedef void __MCF_cxa_dtor_cdecl(void* __arg);

#if defined __GNUC__ || defined __clang__
/* Support both calling conventions with a transparent union.  */
typedef void __thiscall __MCF_cxa_dtor_thiscall(void* __arg);
typedef union __MCF_cxa_dtor_any __MCF_cxa_dtor_any_t;
union __MCF_C(__attribute__((__transparent_union__))) __MCF_cxa_dtor_any
  {
    __MCF_atexit_callback* __no_arg_ptr;
    __MCF_cxa_dtor_cdecl* __cdecl_ptr;
    __MCF_cxa_dtor_thiscall* __thiscall_ptr;

    /* Unfortunately, transparent unions are not supported in C++, and have
     * to be emulated with constructors.  */
    __MCF_CXX(
      __MCF_CXX11(constexpr)
      __MCF_cxa_dtor_any(__MCF_atexit_callback* __xptr) __MCF_noexcept
        : __no_arg_ptr(__xptr)
        { })

    __MCF_CXX(
      __MCF_CXX11(constexpr)
      __MCF_cxa_dtor_any(__MCF_cxa_dtor_cdecl* __xptr) __MCF_noexcept
        : __cdecl_ptr(__xptr)
        { })

# ifdef __i386__
    __MCF_CXX(
      __MCF_CXX11(constexpr)
      __MCF_cxa_dtor_any(__MCF_cxa_dtor_thiscall* __xptr) __MCF_noexcept
        : __thiscall_ptr(__xptr)
        { })
# endif
  };
#else
/* Make these barely compile.  */
typedef __MCF_cxa_dtor_cdecl __MCF_cxa_dtor_thiscall;
typedef __MCF_cxa_dtor_cdecl* __MCF_cxa_dtor_any_t;
#endif

/* Gets the last error number, like `GetLastError()`.  */
__MCF_FWD_IMPORT __MCF_FN_PURE
uint32_t
_MCF_get_win32_error(void) __MCF_noexcept;

/* Gets the system page size, which is usually 4KiB or 8KiB.  */
__MCF_FWD_IMPORT __MCF_FN_CONST
size_t
_MCF_get_page_size(void) __MCF_noexcept;

/* Gets the number of logical processors in the current group.  */
__MCF_FWD_IMPORT __MCF_FN_CONST
size_t
_MCF_get_processor_count(void) __MCF_noexcept;

/* Gets the mask of active processors. Each bit 1 denotes a processor that
 * has been configured into the system.  */
__MCF_FWD_IMPORT __MCF_FN_CONST
uintptr_t
_MCF_get_active_processor_mask(void) __MCF_noexcept;

/* Declare some helper functions.  */
__MCF_ALWAYS_INLINE __MCF_CXX11(constexpr)
size_t
_MCF_minz(size_t __x, size_t __y) __MCF_noexcept
  { return (__y < __x) ? __y : __x;  }

__MCF_ALWAYS_INLINE __MCF_CXX11(constexpr)
size_t
_MCF_maxz(size_t __x, size_t __y) __MCF_noexcept
  { return (__x < __y) ? __y : __x;  }

__MCF_FWD_IMPORT __MCF_NEVER_RETURN __MCF_NEVER_INLINE __MCF_FN_COLD
void
__MCF_runtime_failure(const char* __where);

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_FWD_  */
