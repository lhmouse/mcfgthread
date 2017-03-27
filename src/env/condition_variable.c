// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2017, LH_Mouse. All wrongs reserved.

#define __MCFCRT_CONDITION_VARIABLE_INLINE_OR_EXTERN     extern inline
#include "condition_variable.h"
#include "_nt_timeout.h"
#include "../ext/assert.h"
#include "../ext/expect.h"
#include <winternl.h>
#include <ntstatus.h>

__attribute__((__dllimport__, __stdcall__))
extern NTSTATUS NtWaitForKeyedEvent(HANDLE hKeyedEvent, void *pKey, BOOLEAN bAlertable, const LARGE_INTEGER *pliTimeout);
__attribute__((__dllimport__, __stdcall__))
extern NTSTATUS NtReleaseKeyedEvent(HANDLE hKeyedEvent, void *pKey, BOOLEAN bAlertable, const LARGE_INTEGER *pliTimeout);

__attribute__((__dllimport__, __stdcall__, __const__))
extern BOOLEAN RtlDllShutdownInProgress(void);

#define MASK_WAITING            ((uintptr_t) 0x0001)
#define MASK_THREADS_SPINNING   ((uintptr_t) 0x000C)
#define MASK_THREADS_TRAPPED    ((uintptr_t)~0x000F)

#define THREADS_SPINNING_ONE    ((uintptr_t)(MASK_THREADS_SPINNING & -MASK_THREADS_SPINNING))
#define THREADS_SPINNING_MAX    ((uintptr_t)(MASK_THREADS_SPINNING / THREADS_SPINNING_ONE))

#define THREADS_TRAPPED_ONE     ((uintptr_t)(MASK_THREADS_TRAPPED & -MASK_THREADS_TRAPPED))
#define THREADS_TRAPPED_MAX     ((uintptr_t)(MASK_THREADS_TRAPPED / THREADS_TRAPPED_ONE))

__attribute__((__always_inline__))
static inline bool ReallyWaitForConditionVariable(volatile uintptr_t *puControl,
	_MCFCRT_ConditionVariableUnlockCallback pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback pfnRelockCallback, intptr_t nContext,
	size_t uMaxSpinCount, bool bMayTimeOut, uint64_t u64UntilFastMonoClock, bool bRelockIfTimeOut)
{
	bool bSignaled, bSpinnable = false;
	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
		do {
			if(uMaxSpinCount != 0){
				const size_t uThreadsSpinning = (uOld & MASK_THREADS_SPINNING) / THREADS_SPINNING_ONE;
				bSpinnable = (uThreadsSpinning < THREADS_SPINNING_MAX);
			}
			if(!bSpinnable){
				const bool bNewWaiter = !(uOld & MASK_WAITING);
				if(!bNewWaiter){
					break;
				}
				uNew = uOld | MASK_WAITING;
			} else {
				uNew = (uOld | MASK_WAITING) + THREADS_SPINNING_ONE;
			}
		} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)));
	}
	const intptr_t nUnlocked = (*pfnUnlockCallback)(nContext);
	if(bSpinnable){
		for(size_t i = 0; i < uMaxSpinCount; ++i){
			{
				uintptr_t uOld, uNew;
				uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
				do {
					bSignaled = !(uOld & MASK_WAITING);
					if(!bSignaled){
						break;
					}
					uNew = uOld & ~MASK_THREADS_SPINNING;
				} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)));
			}
			if(_MCFCRT_EXPECT_NOT(bSignaled)){
				(*pfnRelockCallback)(nContext, nUnlocked);
				return true;
			}
			__builtin_ia32_pause();
		}
	}

	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
		do {
			bSignaled = !(uOld & MASK_WAITING);
			if(!bSignaled){
				uNew = uOld + THREADS_TRAPPED_ONE;
			} else {
				uNew = uOld & ~MASK_THREADS_SPINNING;
			}
		} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)));
	}
	if(_MCFCRT_EXPECT_NOT(bSignaled)){
		(*pfnRelockCallback)(nContext, nUnlocked);
		return true;
	}
	if(bMayTimeOut){
		LARGE_INTEGER liTimeout;
		__MCFCRT_InitializeNtTimeout(&liTimeout, u64UntilFastMonoClock);
		NTSTATUS lStatus = NtWaitForKeyedEvent(_MCFCRT_NULLPTR, (void *)puControl, false, &liTimeout);
		_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() failed.");
		while(_MCFCRT_EXPECT(lStatus == STATUS_TIMEOUT)){
			bool bDecremented;
			{
				uintptr_t uOld, uNew;
				uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
				do {
					const size_t uThreadsTrapped = (uOld & MASK_THREADS_TRAPPED) / THREADS_TRAPPED_ONE;
					bDecremented = (uThreadsTrapped > 0);
					if(!bDecremented){
						break;
					}
					uNew = uOld - THREADS_TRAPPED_ONE;
				} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)));
			}
			if(bDecremented){
				if(bRelockIfTimeOut){
					(*pfnRelockCallback)(nContext, nUnlocked);
				}
				return false;
			}
			liTimeout.QuadPart = 0;
			lStatus = NtWaitForKeyedEvent(_MCFCRT_NULLPTR, (void *)puControl, false, &liTimeout);
			_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() failed.");
		}
	} else {
		NTSTATUS lStatus = NtWaitForKeyedEvent(_MCFCRT_NULLPTR, (void *)puControl, false, _MCFCRT_NULLPTR);
		_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() failed.");
		_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
	}
	(*pfnRelockCallback)(nContext, nUnlocked);
	return true;
}
__attribute__((__always_inline__))
static inline size_t ReallySignalConditionVariable(volatile uintptr_t *puControl, size_t uMaxCountToSignal){
	uintptr_t uCountToSignal;
	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
		do {
			uNew = uOld & ~(MASK_WAITING | MASK_THREADS_SPINNING);
			const size_t uThreadsTrapped = (uOld & MASK_THREADS_TRAPPED) / THREADS_TRAPPED_ONE;
			uCountToSignal = (uThreadsTrapped <= uMaxCountToSignal) ? uThreadsTrapped : uMaxCountToSignal;
			uNew -= uCountToSignal * THREADS_TRAPPED_ONE;
		} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)));
	}
	// If `RtlDllShutdownInProgress()` is `true`, other threads will have been terminated.
	// Calling `NtReleaseKeyedEvent()` when no thread is waiting results in deadlocks. Don't do that.
	if((uCountToSignal > 0) && !RtlDllShutdownInProgress()){
		for(size_t i = 0; i < uCountToSignal; ++i){
			NTSTATUS lStatus = NtReleaseKeyedEvent(_MCFCRT_NULLPTR, (void *)puControl, false, _MCFCRT_NULLPTR);
			_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtReleaseKeyedEvent() failed.");
			_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
		}
	}
	return uCountToSignal;
}

bool _MCFCRT_WaitForConditionVariable(_MCFCRT_ConditionVariable *pConditionVariable,
	_MCFCRT_ConditionVariableUnlockCallback pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback pfnRelockCallback, intptr_t nContext,
	size_t uMaxSpinCount, uint64_t u64UntilFastMonoClock)
{
	const bool bSignaled = ReallyWaitForConditionVariable(&(pConditionVariable->__u), pfnUnlockCallback, pfnRelockCallback, nContext,
		uMaxSpinCount, true, u64UntilFastMonoClock, true);
	return bSignaled;
}
bool _MCFCRT_WaitForConditionVariableOrAbandon(_MCFCRT_ConditionVariable *pConditionVariable,
	_MCFCRT_ConditionVariableUnlockCallback pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback pfnRelockCallback, intptr_t nContext,
	size_t uMaxSpinCount, uint64_t u64UntilFastMonoClock)
{
	const bool bSignaled = ReallyWaitForConditionVariable(&(pConditionVariable->__u), pfnUnlockCallback, pfnRelockCallback, nContext,
		uMaxSpinCount, true, u64UntilFastMonoClock, false);
	return bSignaled;
}
void _MCFCRT_WaitForConditionVariableForever(_MCFCRT_ConditionVariable *pConditionVariable,
	_MCFCRT_ConditionVariableUnlockCallback pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback pfnRelockCallback, intptr_t nContext,
	size_t uMaxSpinCount)
{
	const bool bSignaled = ReallyWaitForConditionVariable(&(pConditionVariable->__u), pfnUnlockCallback, pfnRelockCallback, nContext,
		uMaxSpinCount, false, UINT64_MAX, true);
	_MCFCRT_ASSERT(bSignaled);
}
size_t _MCFCRT_SignalConditionVariable(_MCFCRT_ConditionVariable *pConditionVariable, size_t uMaxCountToSignal){
	return ReallySignalConditionVariable(&(pConditionVariable->__u), uMaxCountToSignal);
}
size_t _MCFCRT_BroadcastConditionVariable(_MCFCRT_ConditionVariable *pConditionVariable){
	return ReallySignalConditionVariable(&(pConditionVariable->__u), SIZE_MAX);
}
