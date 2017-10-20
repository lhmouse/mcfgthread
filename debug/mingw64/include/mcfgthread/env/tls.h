// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2017, LH_Mouse. All wrongs reserved.

// FIXME: This file should have been placed in 'src/pre/' as all TLS stuff is module-specific.

#ifndef __MCFCRT_PRE_TLS_H_
#define __MCFCRT_PRE_TLS_H_

#include "_crtdef.h"
#include "_tls_common.h"

_MCFCRT_EXTERN_C_BEGIN

extern bool __MCFCRT_TlsInit(void) _MCFCRT_NOEXCEPT;
extern void __MCFCRT_TlsUninit(void) _MCFCRT_NOEXCEPT;

extern void __MCFCRT_TlsCleanup(void) _MCFCRT_NOEXCEPT;

extern bool _MCFCRT_TlsGet(_MCFCRT_TlsKeyHandle __hTlsKey, void **_MCFCRT_RESTRICT __ppStorage) _MCFCRT_NOEXCEPT;
extern bool _MCFCRT_TlsRequire(_MCFCRT_TlsKeyHandle __hTlsKey, void **_MCFCRT_RESTRICT __ppStorage) _MCFCRT_NOEXCEPT;

extern bool _MCFCRT_AtThreadExit(_MCFCRT_AtThreadExitCallback __pfnProc, _MCFCRT_STD intptr_t __nContext) _MCFCRT_NOEXCEPT;

_MCFCRT_EXTERN_C_END

#endif
