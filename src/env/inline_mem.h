// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2019, LH_Mouse. All wrongs reserved.

#ifndef __MCFCRT_ENV_INLINE_MEM_H_
#define __MCFCRT_ENV_INLINE_MEM_H_

#include "_crtdef.h"
#include "xassert.h"

_MCFCRT_EXTERN_C_BEGIN

__attribute__((__always_inline__)) static inline void * _MCFCRT_inline_mempcpy_fwd(void *__s1, const void *__s2, _MCFCRT_STD size_t __n) _MCFCRT_NOEXCEPT
  {
    _MCFCRT_STD uint8_t *__p1 = (_MCFCRT_STD uint8_t *)__s1;
    const _MCFCRT_STD uint8_t *__p2 = (const _MCFCRT_STD uint8_t *)__s2;
    _MCFCRT_STD size_t __dumb;
  #ifdef _WIN64
    if(__builtin_constant_p(__n) ? (__n / 8) : true) {
      __asm__ (
        "rep movsq \n"
        : "=o"(*(char (*)[])__p1), "+D"(__p1), "+S"(__p2), "=c"(__dumb)
        : "o"(*(const char (*)[])__p2), "c"(__n / 8)
      );
    }
    if(__builtin_constant_p(__n) ? (__n % 8) : true) {
      __asm__ (
        "rep movsb \n"
        : "=o"(*(char (*)[])__p1), "+D"(__p1), "+S"(__p2), "=c"(__dumb)
        : "o"(*(const char (*)[])__p2), "c"(__n % 8)
      );
    }
  #else
    if(__builtin_constant_p(__n) ? (__n / 4) : true) {
      __asm__ (
        "rep movs{l|d} \n"
        : "=o"(*(char (*)[])__p1), "+D"(__p1), "+S"(__p2), "=c"(__dumb)
        : "o"(*(const char (*)[])__p2), "c"(__n / 4)
      );
    }
    if(__builtin_constant_p(__n) ? (__n % 4) : true) {
      __asm__ (
        "rep movsb \n"
        : "=o"(*(char (*)[])__p1), "+D"(__p1), "+S"(__p2), "=c"(__dumb)
        : "o"(*(const char (*)[])__p2), "c"(__n % 4)
      );
    }
  #endif
    _MCFCRT_ASSERT(__p1 == (_MCFCRT_STD uint8_t *)__s1 + __n);
    _MCFCRT_ASSERT(__p2 == (_MCFCRT_STD uint8_t *)__s2 + __n);
    return __p1;
  }

__attribute__((__always_inline__)) static inline void * _MCFCRT_inline_mempset_fwd(void *__s, int __c, _MCFCRT_STD size_t __n) _MCFCRT_NOEXCEPT
  {
    _MCFCRT_STD uint8_t *__p = (_MCFCRT_STD uint8_t *)__s;
    _MCFCRT_STD uintptr_t __word = (_MCFCRT_STD uint8_t)__c;
    _MCFCRT_STD size_t __dumb;
#ifdef _WIN64
    if(__builtin_constant_p(__n) ? (__n / 8) : true) {
      __word += __word <<  8;
      __word += __word << 16;
      __word += __word << 32;
      __asm__ (
        "rep stosq \n"
        : "=o"(*(char (*)[])__p), "+D"(__p), "=c"(__dumb)
        : "a"(__word), "c"(__n / 8)
      );
    }
    if(__builtin_constant_p(__n) ? (__n % 8) : true) {
      __asm__ (
        "rep stosb \n"
        : "=o"(*(char (*)[])__p), "+D"(__p), "=c"(__dumb)
        : "a"(__word), "c"(__n % 8)
      );
    }
#else
    if(__builtin_constant_p(__n) ? (__n / 4) : true) {
      __word += __word <<  8;
      __word += __word << 16;
      __asm__ (
        "rep stos{l|d} \n"
        : "=o"(*(char (*)[])__p), "+D"(__p), "=c"(__dumb)
        : "a"(__word), "c"(__n / 4)
      );
    }
    if(__builtin_constant_p(__n) ? (__n % 4) : true) {
      __asm__ (
        "rep stosb \n"
        : "=o"(*(char (*)[])__p), "+D"(__p), "=c"(__dumb)
        : "a"(__word), "c"(__n % 4)
      );
    }
#endif
    _MCFCRT_ASSERT(__p == (_MCFCRT_STD uint8_t *)__s + __n);
    return __p;
  }

_MCFCRT_EXTERN_C_END

#endif
