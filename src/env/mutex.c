// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#define __MCFCRT_MUTEX_INLINE_OR_EXTERN     extern inline
#include "mutex.h"
#include "_nt_timeout.h"
#include "../ext/assert.h"
#include "../ext/expect.h"
#include <limits.h>
#include <winternl.h>
#include <ntstatus.h>

__attribute__((__dllimport__, __stdcall__))
extern NTSTATUS NtWaitForKeyedEvent(HANDLE hKeyedEvent, void *pKey, BOOLEAN bAlertable, const LARGE_INTEGER *pliTimeout);
__attribute__((__dllimport__, __stdcall__))
extern NTSTATUS NtReleaseKeyedEvent(HANDLE hKeyedEvent, void *pKey, BOOLEAN bAlertable, const LARGE_INTEGER *pliTimeout);

#define MASK_LOCKED             ((uintptr_t) 0x0001)
#define MASK_THREADS_SPINNING   ((uintptr_t) 0x000C)
#define MASK_THREADS_TRAPPED    ((uintptr_t)~0x000F)

#define THREAD_SPINNING_ONE     ((uintptr_t)(MASK_THREADS_SPINNING & -MASK_THREADS_SPINNING))
#define THREAD_SPINNING_MAX     ((uintptr_t)(MASK_THREADS_SPINNING / THREAD_SPINNING_ONE))

#define THREAD_TRAPPED_ONE      ((uintptr_t)(MASK_THREADS_TRAPPED & -MASK_THREADS_TRAPPED))
#define THREAD_TRAPPED_MAX      ((uintptr_t)(MASK_THREADS_TRAPPED / THREAD_TRAPPED_ONE))

__attribute__((__always_inline__))
static inline bool ReallyTryMutex(volatile uintptr_t *puControl){
	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
		if(_MCFCRT_EXPECT(!(uOld & MASK_LOCKED))){
			uNew = uOld | MASK_LOCKED;
			if(_MCFCRT_EXPECT(__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))){
				return true;
			}
		}
	}
	return false;
}
__attribute__((__always_inline__))
static inline bool ReallyWaitForMutex(volatile uintptr_t *puControl, size_t uMaxSpinCount, bool bMayTimeOut, uint64_t u64UntilFastMonoClock){
	for(;;){
		bool bTaken, bSpinnable = false;
		if(uMaxSpinCount != 0){
			{
				uintptr_t uOld, uNew;
				uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
				do {
					bTaken = !(uOld & MASK_LOCKED);
					if(!bTaken){
						const size_t uThreadsSpinning = (uOld & MASK_THREADS_SPINNING) / THREAD_SPINNING_ONE;
						bSpinnable = (uThreadsSpinning < THREAD_SPINNING_MAX);
						if(!bSpinnable){
							break;
						}
						uNew = uOld + THREAD_SPINNING_ONE;
					} else {
						uNew = (uOld & ~MASK_THREADS_SPINNING) | MASK_LOCKED;
					}
				} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
			}
			if(_MCFCRT_EXPECT(bTaken)){
				return true;
			}
		}
		if(bSpinnable){
			for(size_t i = 0; i < uMaxSpinCount; ++i){
				{
					uintptr_t uOld, uNew;
					uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
					do {
						bTaken = !(uOld & MASK_LOCKED);
						if(!bTaken){
							break;
						}
						uNew = (uOld & ~MASK_THREADS_SPINNING) | MASK_LOCKED;
					} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
				}
				if(_MCFCRT_EXPECT_NOT(bTaken)){
					return true;
				}
				__builtin_ia32_pause();
			}
		}

		{
			uintptr_t uOld, uNew;
			uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
			do {
				bTaken = !(uOld & MASK_LOCKED);
				if(!bTaken){
					uNew = uOld + THREAD_TRAPPED_ONE;
				} else {
					uNew = (uOld & ~MASK_THREADS_SPINNING) | MASK_LOCKED;
				}
			} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
		}
		if(_MCFCRT_EXPECT(bTaken)){
			return true;
		}
		if(bMayTimeOut){
			LARGE_INTEGER liTimeout;
			__MCFCRT_InitializeNtTimeout(&liTimeout, u64UntilFastMonoClock);
			NTSTATUS lStatus = NtWaitForKeyedEvent(nullptr, (void *)puControl, false, &liTimeout);
			_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() failed.");
			while(_MCFCRT_EXPECT(lStatus == STATUS_TIMEOUT)){
				bool bDecremented;
				{
					uintptr_t uOld, uNew;
					uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
					do {
						const size_t uThreadsTrapped = (uOld & MASK_THREADS_TRAPPED) / THREAD_TRAPPED_ONE;
						bDecremented = (uThreadsTrapped > 0);
						if(!bDecremented){
							break;
						}
						uNew = uOld - THREAD_TRAPPED_ONE;
					} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
				}
				if(bDecremented){
					return false;
				}
				liTimeout.QuadPart = 0;
				lStatus = NtWaitForKeyedEvent(nullptr, (void *)puControl, false, &liTimeout);
				_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() failed.");
			}
		} else {
			NTSTATUS lStatus = NtWaitForKeyedEvent(nullptr, (void *)puControl, false, nullptr);
			_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() failed.");
			_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
		}
	}
}
__attribute__((__always_inline__))
static inline void ReallySignalMutex(volatile uintptr_t *puControl){
	bool bSignalOne;
	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
		do {
			_MCFCRT_ASSERT_MSG(uOld & MASK_LOCKED, L"This mutex isn't locked by any thread.");

			uNew = uOld & ~(MASK_LOCKED | MASK_THREADS_SPINNING);
			bSignalOne = (uOld & MASK_THREADS_TRAPPED) > 0;
			if(bSignalOne){
				uNew -= THREAD_TRAPPED_ONE;
			}
		} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
	}
	if(bSignalOne){
		NTSTATUS lStatus = NtReleaseKeyedEvent(nullptr, (void *)puControl, false, nullptr);
		_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtReleaseKeyedEvent() failed.");
		_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
	}
}

bool _MCFCRT_WaitForMutex(_MCFCRT_Mutex *pMutex, size_t uMaxSpinCount, uint64_t u64UntilFastMonoClock){
	bool bLocked = ReallyTryMutex(&(pMutex->__u));
	if(_MCFCRT_EXPECT_NOT(!bLocked) && _MCFCRT_EXPECT_NOT(u64UntilFastMonoClock != 0)){
		bLocked = ReallyWaitForMutex(&(pMutex->__u), uMaxSpinCount, true, u64UntilFastMonoClock);
	}
	return bLocked;
}
void _MCFCRT_WaitForMutexForever(_MCFCRT_Mutex *pMutex, size_t uMaxSpinCount){
	bool bLocked = ReallyTryMutex(&(pMutex->__u));
	if(_MCFCRT_EXPECT_NOT(!bLocked)){
		bLocked = ReallyWaitForMutex(&(pMutex->__u), uMaxSpinCount, false, UINT64_MAX);
	}
	_MCFCRT_ASSERT(bLocked);
}
void _MCFCRT_SignalMutex(_MCFCRT_Mutex *pMutex){
	ReallySignalMutex(&(pMutex->__u));
}
