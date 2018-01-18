// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2018, LH_Mouse. All wrongs reserved.

#include "tls.h"
#include "mcfwin.h"
#include "xassert.h"

static DWORD g_dwTlsIndex = TLS_OUT_OF_INDEXES;

bool __MCFCRT_TlsInit(void){
	const DWORD dwTlsIndex = TlsAlloc();
	if(dwTlsIndex == TLS_OUT_OF_INDEXES){
		return false;
	}

	g_dwTlsIndex = dwTlsIndex;
	return true;
}
void __MCFCRT_TlsUninit(void){
	__MCFCRT_TlsCleanup();

	const DWORD dwTlsIndex = g_dwTlsIndex;
	g_dwTlsIndex = TLS_OUT_OF_INDEXES;

	const bool bSucceeded = TlsFree(dwTlsIndex);
	_MCFCRT_ASSERT(bSucceeded);
}

void __MCFCRT_TlsCleanup(void){
	const DWORD dwTlsIndex = g_dwTlsIndex;
	_MCFCRT_ASSERT(dwTlsIndex != TLS_OUT_OF_INDEXES);

	__MCFCRT_TlsThreadMapHandle hThreadMap = TlsGetValue(dwTlsIndex);
	if(!hThreadMap){
		return;
	}
	const bool bSucceeded = TlsSetValue(dwTlsIndex, _MCFCRT_NULLPTR);
	_MCFCRT_ASSERT(bSucceeded);

	__MCFCRT_InternalTlsDestroyThreadMap(hThreadMap);
}

bool _MCFCRT_TlsGet(_MCFCRT_TlsKeyHandle hTlsKey, void **restrict ppStorage){
	const DWORD dwTlsIndex = g_dwTlsIndex;
	_MCFCRT_ASSERT(dwTlsIndex != TLS_OUT_OF_INDEXES);

#ifndef NDEBUG
	*ppStorage = (void *)0xDEADBEEF;
#endif
	DWORD dwErrorCode;

	__MCFCRT_TlsThreadMapHandle hThreadMap = TlsGetValue(dwTlsIndex);
	if(!hThreadMap){
		return false;
	}
	dwErrorCode = __MCFCRT_InternalTlsGet(hThreadMap, hTlsKey, ppStorage);
	if(dwErrorCode != 0){
		SetLastError(dwErrorCode);
		return false;
	}
	return true;
}
bool _MCFCRT_TlsRequire(_MCFCRT_TlsKeyHandle hTlsKey, void **restrict ppStorage){
	const DWORD dwTlsIndex = g_dwTlsIndex;
	_MCFCRT_ASSERT(dwTlsIndex != TLS_OUT_OF_INDEXES);

#ifndef NDEBUG
	*ppStorage = (void *)0xDEADBEEF;
#endif
	DWORD dwErrorCode;

	__MCFCRT_TlsThreadMapHandle hThreadMap = TlsGetValue(dwTlsIndex);
	if(!hThreadMap){
		hThreadMap = __MCFCRT_InternalTlsCreateThreadMap();
		if(!hThreadMap){
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return false;
		}
		if(!TlsSetValue(dwTlsIndex, hThreadMap)){
			dwErrorCode = GetLastError();
			__MCFCRT_InternalTlsDestroyThreadMap(hThreadMap);
			SetLastError(dwErrorCode);
			return false;
		}
	}
	dwErrorCode = __MCFCRT_InternalTlsRequire(hThreadMap, hTlsKey, ppStorage);
	if(dwErrorCode != 0){
		SetLastError(dwErrorCode);
		return false;
	}
	return true;
}

bool _MCFCRT_AtThreadExit(_MCFCRT_AtThreadExitCallback pfnProc, intptr_t nContext){
	const DWORD dwTlsIndex = g_dwTlsIndex;
	_MCFCRT_ASSERT(dwTlsIndex != TLS_OUT_OF_INDEXES);

	DWORD dwErrorCode;

	__MCFCRT_TlsThreadMapHandle hThreadMap = TlsGetValue(dwTlsIndex);
	if(!hThreadMap){
		hThreadMap = __MCFCRT_InternalTlsCreateThreadMap();
		if(!hThreadMap){
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return false;
		}
		if(!TlsSetValue(dwTlsIndex, hThreadMap)){
			dwErrorCode = GetLastError();
			__MCFCRT_InternalTlsDestroyThreadMap(hThreadMap);
			SetLastError(dwErrorCode);
			return false;
		}
	}
	dwErrorCode = __MCFCRT_InternalAtThreadExit(hThreadMap, pfnProc, nContext);
	if(dwErrorCode != 0){
		SetLastError(dwErrorCode);
		return false;
	}
	return true;
}
