// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2018, LH_Mouse. All wrongs reserved.

#ifndef __MCFCRT_ENV_TLS_COMMON_H_
#define __MCFCRT_ENV_TLS_COMMON_H_

#include "_crtdef.h"

_MCFCRT_EXTERN_C_BEGIN

typedef unsigned long (*_MCFCRT_TlsConstructor)(_MCFCRT_STD intptr_t __nContext, void *__pStorage);
typedef void (*_MCFCRT_TlsDestructor)(_MCFCRT_STD intptr_t __nContext, void *__pStorage);

typedef struct __MCFCRT_tagTlsKeyHandle { int __n; } *_MCFCRT_TlsKeyHandle;

extern _MCFCRT_TlsKeyHandle _MCFCRT_TlsAllocKey(_MCFCRT_STD size_t __uSize, _MCFCRT_TlsConstructor __pfnConstructor, _MCFCRT_TlsDestructor __pfnDestructor, _MCFCRT_STD intptr_t __nContext) _MCFCRT_NOEXCEPT;
extern void _MCFCRT_TlsFreeKey(_MCFCRT_TlsKeyHandle __hTlsKey) _MCFCRT_NOEXCEPT;

extern _MCFCRT_STD size_t _MCFCRT_TlsGetSize(_MCFCRT_TlsKeyHandle __hTlsKey) _MCFCRT_NOEXCEPT;
extern _MCFCRT_TlsConstructor _MCFCRT_TlsGetConstructor(_MCFCRT_TlsKeyHandle __hTlsKey) _MCFCRT_NOEXCEPT;
extern _MCFCRT_TlsDestructor _MCFCRT_TlsGetDestructor(_MCFCRT_TlsKeyHandle __hTlsKey) _MCFCRT_NOEXCEPT;
extern _MCFCRT_STD intptr_t _MCFCRT_TlsGetContext(_MCFCRT_TlsKeyHandle __hTlsKey) _MCFCRT_NOEXCEPT;

typedef struct __MCFCRT_tagTlsThreadMapHandle { int __n; } *__MCFCRT_TlsThreadMapHandle;

extern __MCFCRT_TlsThreadMapHandle __MCFCRT_InternalTlsCreateThreadMap(void) _MCFCRT_NOEXCEPT;
extern void __MCFCRT_InternalTlsDestroyThreadMap(__MCFCRT_TlsThreadMapHandle __hThreadMap) _MCFCRT_NOEXCEPT;

extern unsigned long __MCFCRT_InternalTlsGet(__MCFCRT_TlsThreadMapHandle __hThreadMap, _MCFCRT_TlsKeyHandle __hTlsKey, void **_MCFCRT_RESTRICT __ppStorage) _MCFCRT_NOEXCEPT;
extern unsigned long __MCFCRT_InternalTlsRequire(__MCFCRT_TlsThreadMapHandle __hThreadMap, _MCFCRT_TlsKeyHandle __hTlsKey, void **_MCFCRT_RESTRICT __ppStorage) _MCFCRT_NOEXCEPT;

// Thread exit callbacks are thread-local anyway...
typedef void (*_MCFCRT_AtThreadExitCallback)(_MCFCRT_STD intptr_t __nContext);

extern unsigned long __MCFCRT_InternalAtThreadExit(__MCFCRT_TlsThreadMapHandle __hThreadMap, _MCFCRT_AtThreadExitCallback __pfnProc, _MCFCRT_STD intptr_t __nContext) _MCFCRT_NOEXCEPT;

_MCFCRT_EXTERN_C_END

#endif
