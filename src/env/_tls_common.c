// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2017, LH_Mouse. All wrongs reserved.

#include "_tls_common.h"
#include "mutex.h"
#include "avl_tree.h"
#include "heap.h"
#include "inline_mem.h"
#include "mcfwin.h"

typedef struct tagTlsKey {
	uintptr_t uCounter;

	size_t uSize;
	_MCFCRT_TlsConstructor pfnConstructor;
	_MCFCRT_TlsDestructor pfnDestructor;
	intptr_t nContext;
} TlsKey;

_MCFCRT_TlsKeyHandle _MCFCRT_TlsAllocKey(size_t uSize, _MCFCRT_TlsConstructor pfnConstructor, _MCFCRT_TlsDestructor pfnDestructor, intptr_t nContext){
	static volatile size_t s_uKeyCounter;

	TlsKey *const pKey = _MCFCRT_malloc(sizeof(TlsKey));
	if(!pKey){
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return _MCFCRT_NULLPTR;
	}
	pKey->uCounter       = __atomic_add_fetch(&s_uKeyCounter, 1, __ATOMIC_RELAXED);
	pKey->uSize          = uSize;
	pKey->pfnConstructor = pfnConstructor;
	pKey->pfnDestructor  = pfnDestructor;
	pKey->nContext       = nContext;

	return (_MCFCRT_TlsKeyHandle)pKey;
}
void _MCFCRT_TlsFreeKey(_MCFCRT_TlsKeyHandle hTlsKey){
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	if(!pKey){
		return;
	}

	_MCFCRT_free(pKey);
}

size_t _MCFCRT_TlsGetSize(_MCFCRT_TlsKeyHandle hTlsKey){
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	return pKey->uSize;
}
_MCFCRT_TlsConstructor _MCFCRT_TlsGetConstructor(_MCFCRT_TlsKeyHandle hTlsKey){
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	return pKey->pfnConstructor;
}
_MCFCRT_TlsDestructor _MCFCRT_TlsGetDestructor(_MCFCRT_TlsKeyHandle hTlsKey){
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	return pKey->pfnDestructor;
}
intptr_t _MCFCRT_TlsGetContext(_MCFCRT_TlsKeyHandle hTlsKey){
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	return pKey->nContext;
}

typedef struct tagTlsObjectKey {
	struct tagTlsKey *pKey;
	uintptr_t uCounter;
} TlsObjectKey;

typedef struct tagTlsObject {
	_MCFCRT_AvlNodeHeader avlhNodeByKey;

	_MCFCRT_TlsDestructor pfnDestructor;
	intptr_t nContext;

	struct tagTlsObject *pPrev; // By thread
	struct tagTlsObject *pNext; // By thread
	struct tagTlsObjectKey vObjectKey;

	alignas(max_align_t) unsigned char abyStorage[];
} TlsObject;

static inline int TlsObjectComparatorNodeKey(const _MCFCRT_AvlNodeHeader *pObjSelf, intptr_t nKeyOther){
	const TlsObjectKey *const pIndexSelf = &((const TlsObject *)pObjSelf)->vObjectKey;
	const TlsObjectKey *const pIndexOther = (const TlsObjectKey *)nKeyOther;
	if(pIndexSelf->pKey != pIndexOther->pKey){
		return (pIndexSelf->pKey < pIndexOther->pKey) ? -1 : 1;
	}
	if(pIndexSelf->uCounter != pIndexOther->uCounter){
		return (pIndexSelf->uCounter < pIndexOther->uCounter) ? -1 : 1;
	}
	return 0;
}
static inline int TlsObjectComparatorNodes(const _MCFCRT_AvlNodeHeader *pObjSelf, const _MCFCRT_AvlNodeHeader *pObjOther){
	return TlsObjectComparatorNodeKey(pObjSelf, (intptr_t)&((const TlsObject *)pObjOther)->vObjectKey);
}

typedef struct tagTlsThreadMap {
	_MCFCRT_AvlRoot avlObjects;
	struct tagTlsObject *pLast; // By thread
	struct tagTlsObject *pFirst; // By thread
} TlsThreadMap;

_MCFCRT_TlsThreadMapHandle __MCFCRT_InternalTlsCreateThreadMap(void){
	TlsThreadMap *const pThreadMap = _MCFCRT_malloc(sizeof(TlsThreadMap));
	if(!pThreadMap){
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return _MCFCRT_NULLPTR;
	}
	pThreadMap->avlObjects = _MCFCRT_NULLPTR;
	pThreadMap->pLast      = _MCFCRT_NULLPTR;
	pThreadMap->pFirst     = _MCFCRT_NULLPTR;

	return (_MCFCRT_TlsThreadMapHandle)pThreadMap;
}
void __MCFCRT_InternalTlsDestroyThreadMap(_MCFCRT_TlsThreadMapHandle hThreadMap){
	TlsThreadMap *const pThreadMap = (TlsThreadMap *)hThreadMap;
	if(!pThreadMap){
		return;
	}

	for(;;){
		TlsObject *const pObject = pThreadMap->pLast;
		if(!pObject){
			break;
		}

		TlsObject *const pPrev = pObject->pPrev;
		TlsObject *const pNext = pObject->pNext;
		_MCFCRT_ASSERT(!pNext);
		if(pPrev){
			pPrev->pNext = pNext;
		} else {
			pThreadMap->pFirst = pNext;
		}
		if(pNext){
			pNext->pPrev = pPrev;
		} else {
			pThreadMap->pLast = pPrev;
		}

		const _MCFCRT_TlsDestructor pfnDestructor = pObject->pfnDestructor;
		if(pfnDestructor){
			(*pfnDestructor)(pObject->nContext, pObject->abyStorage);
		}
		_MCFCRT_free(pObject);
	}

	_MCFCRT_free(pThreadMap);
}

bool __MCFCRT_InternalTlsGet(_MCFCRT_TlsThreadMapHandle hThreadMap, _MCFCRT_TlsKeyHandle hTlsKey, void **restrict ppStorage){
	TlsThreadMap *const pThreadMap = (TlsThreadMap *)hThreadMap;
	_MCFCRT_ASSERT(pThreadMap);
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	_MCFCRT_ASSERT(pKey);

	const TlsObjectKey vObjectKey = { pKey, pKey->uCounter };
	TlsObject *pObject = (TlsObject *)_MCFCRT_AvlFind(&(pThreadMap->avlObjects), (intptr_t)&vObjectKey, &TlsObjectComparatorNodeKey);
	if(!pObject){
		SetLastError(ERROR_FILE_NOT_FOUND);
		return false;
	}
	*ppStorage = pObject->abyStorage;
	return true;
}
bool __MCFCRT_InternalTlsRequire(_MCFCRT_TlsThreadMapHandle hThreadMap, _MCFCRT_TlsKeyHandle hTlsKey, void **restrict ppStorage){
	TlsThreadMap *const pThreadMap = (TlsThreadMap *)hThreadMap;
	_MCFCRT_ASSERT(pThreadMap);
	TlsKey *const pKey = (TlsKey *)hTlsKey;
	_MCFCRT_ASSERT(pKey);

#ifndef NDEBUG
	*ppStorage = (void *)0xDEADBEEF;
#endif

	const TlsObjectKey vObjectKey = { pKey, pKey->uCounter };
	TlsObject *pObject = (TlsObject *)_MCFCRT_AvlFind(&(pThreadMap->avlObjects), (intptr_t)&vObjectKey, &TlsObjectComparatorNodeKey);
	if(!pObject){
		const size_t uSizeToAlloc = sizeof(TlsObject) + pKey->uSize;
		if(uSizeToAlloc < sizeof(TlsObject)){
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return false;
		}
		pObject = _MCFCRT_malloc(uSizeToAlloc);
		if(!pObject){
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return false;
		}
#ifndef NDEBUG
		_MCFCRT_inline_mempset_fwd(pObject, 0xAA, sizeof(TlsObject));
#endif
		_MCFCRT_inline_mempset_fwd(pObject->abyStorage, 0, pKey->uSize);
		if(pKey->pfnConstructor){
			const unsigned long ulErrorCode = (*(pKey->pfnConstructor))(pKey->nContext, pObject->abyStorage);
			if(ulErrorCode != 0){
				_MCFCRT_free(pObject);
				SetLastError(ulErrorCode);
				return false;
			}
		}
		pObject->pfnDestructor = pKey->pfnDestructor;
		pObject->nContext      = pKey->nContext;

		TlsObject *const pPrev = pThreadMap->pLast;
		TlsObject *const pNext = _MCFCRT_NULLPTR;
		if(pPrev){
			pPrev->pNext = pObject;
		} else {
			pThreadMap->pFirst = pObject;
		}
		if(pNext){
			pNext->pPrev = pObject;
		} else {
			pThreadMap->pLast = pObject;
		}
		pObject->pPrev = pPrev;
		pObject->pNext = pNext;

		pObject->vObjectKey = vObjectKey;
		_MCFCRT_AvlAttach(&(pThreadMap->avlObjects), (_MCFCRT_AvlNodeHeader *)pObject, &TlsObjectComparatorNodes);
	}
	*ppStorage = pObject->abyStorage;
	return true;
}

#define CALLBACKS_PER_BLOCK   64u

typedef struct tagAtExitElement {
	_MCFCRT_AtThreadExitCallback pfnProc;
	intptr_t nContext;
} AtExitElement;

typedef struct tagAtExitBlock {
	size_t uSize;
	AtExitElement aCallbacks[CALLBACKS_PER_BLOCK];
} AtExitBlock;

static void CrtAtThreadExitDestructor(intptr_t nUnused, void *pStorage){
	(void)nUnused;

	AtExitBlock *const pBlock = pStorage;
	for(size_t uIndex = pBlock->uSize; uIndex != 0; --uIndex){
		(*(pBlock->aCallbacks[uIndex - 1].pfnProc))(pBlock->aCallbacks[uIndex - 1].nContext);
	}
}

bool __MCFCRT_InternalAtThreadExit(_MCFCRT_TlsThreadMapHandle hThreadMap, _MCFCRT_AtThreadExitCallback pfnProc, intptr_t nContext){
	TlsThreadMap *const pThreadMap = (TlsThreadMap *)hThreadMap;
	_MCFCRT_ASSERT(pThreadMap);

	AtExitBlock *pBlock = _MCFCRT_NULLPTR;
	TlsObject *pObject = pThreadMap->pLast;
	if(pObject && (pObject->pfnDestructor == &CrtAtThreadExitDestructor)){
		pBlock = (void *)pObject->abyStorage;
	}
	if(!pBlock || (pBlock->uSize >= CALLBACKS_PER_BLOCK)){
		pObject = _MCFCRT_malloc(sizeof(TlsObject) + sizeof(AtExitBlock));
		if(!pObject){
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return false;
		}
#ifndef NDEBUG
		_MCFCRT_inline_mempset_fwd(pObject, 0xAA, sizeof(TlsObject));
#endif
		pBlock = (void *)pObject->abyStorage;
		pBlock->uSize = 0;

		TlsObject *const pPrev = pThreadMap->pLast;
		TlsObject *const pNext = _MCFCRT_NULLPTR;
		if(pPrev){
			pPrev->pNext = pObject;
		} else {
			pThreadMap->pFirst = pObject;
		}
		if(pNext){
			pNext->pPrev = pObject;
		} else {
			pThreadMap->pLast = pObject;
		}
		pObject->pPrev = pPrev;
		pObject->pNext = pNext;
	}
	const AtExitElement vElement = { pfnProc, nContext };
	pBlock->aCallbacks[(pBlock->uSize)++] = vElement;
	return true;
}
