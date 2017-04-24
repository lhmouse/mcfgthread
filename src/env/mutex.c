// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2017, LH_Mouse. All wrongs reserved.

#define __MCFCRT_MUTEX_INLINE_OR_EXTERN     extern inline
#include "mutex.h"
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

#define MASK_LOCKED             ((uintptr_t) 0x0001)
#define MASK_THREADS_SPINNING   ((uintptr_t) 0x000E)
#define MASK_SPIN_FAILURE_COUNT ((uintptr_t) 0x00F0)
#define MASK_THREADS_TRAPPED    ((uintptr_t)~0x00FF)

#define THREADS_SPINNING_ONE    ((uintptr_t)(MASK_THREADS_SPINNING & -MASK_THREADS_SPINNING))
#define THREADS_SPINNING_MAX    ((uintptr_t)(MASK_THREADS_SPINNING / THREADS_SPINNING_ONE))

#define SPIN_FAILURE_COUNT_ONE  ((uintptr_t)(MASK_SPIN_FAILURE_COUNT & -MASK_SPIN_FAILURE_COUNT))
#define SPIN_FAILURE_COUNT_MAX  ((uintptr_t)(MASK_SPIN_FAILURE_COUNT / SPIN_FAILURE_COUNT_ONE))

#define THREADS_TRAPPED_ONE     ((uintptr_t)(MASK_THREADS_TRAPPED & -MASK_THREADS_TRAPPED))
#define THREADS_TRAPPED_MAX     ((uintptr_t)(MASK_THREADS_TRAPPED / THREADS_TRAPPED_ONE))

#define MIN_SPIN_COUNT          ((uintptr_t)16)

__attribute__((__always_inline__))
static inline bool ReallyWaitForMutex(volatile uintptr_t *puControl, size_t uMaxSpinCountInitial, bool bMayTimeOut, uint64_t u64UntilFastMonoClock){
	for(;;){
		size_t uMaxSpinCount;
		bool bTaken, bSpinnable;
		{
			uintptr_t uOld, uNew;
			uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
			do {
				const size_t uSpinFailureCount = (uOld & MASK_SPIN_FAILURE_COUNT) / SPIN_FAILURE_COUNT_ONE;
				if(uMaxSpinCountInitial > MIN_SPIN_COUNT){
					uMaxSpinCount = (uMaxSpinCountInitial >> uSpinFailureCount * (sizeof(uMaxSpinCountInitial) * CHAR_BIT / (SPIN_FAILURE_COUNT_MAX + 1))) | MIN_SPIN_COUNT;
				} else {
					uMaxSpinCount = uMaxSpinCountInitial;
				}
				bTaken = !(uOld & MASK_LOCKED);
				bSpinnable = false;
				if(!bTaken){
					if(uMaxSpinCount != 0){
						const size_t uThreadsSpinning = (uOld & MASK_THREADS_SPINNING) / THREADS_SPINNING_ONE;
						bSpinnable = uThreadsSpinning < THREADS_SPINNING_MAX;
					}
					if(!bSpinnable){
						break;
					}
					uNew = uOld + THREADS_SPINNING_ONE;
				} else {
					const bool bSpinFailureCountDecremented = uSpinFailureCount != 0;
					uNew = uOld + MASK_LOCKED - bSpinFailureCountDecremented * SPIN_FAILURE_COUNT_ONE;
				}
			} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
		}
		if(_MCFCRT_EXPECT(bTaken)){
			return true;
		}
		if(_MCFCRT_EXPECT(bSpinnable)){
			for(size_t i = 0; _MCFCRT_EXPECT(i < uMaxSpinCount); ++i){
				__builtin_ia32_pause();
#ifdef __SSE2__
				__builtin_ia32_mfence();
#endif
				{
					uintptr_t uOld, uNew;
					uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
					do {
						bTaken = !(uOld & MASK_LOCKED);
						if(!bTaken){
							break;
						}
						const size_t uSpinFailureCount = (uOld & MASK_SPIN_FAILURE_COUNT) / SPIN_FAILURE_COUNT_ONE;
						const bool bSpinFailureCountDecremented = uSpinFailureCount != 0;
						uNew = uOld - THREADS_SPINNING_ONE + MASK_LOCKED - bSpinFailureCountDecremented * SPIN_FAILURE_COUNT_ONE;
					} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
				}
				if(_MCFCRT_EXPECT_NOT(bTaken)){
					return true;
				}
			}
			{
				uintptr_t uOld, uNew;
				uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
				do {
					const size_t uSpinFailureCount = (uOld & MASK_SPIN_FAILURE_COUNT) / SPIN_FAILURE_COUNT_ONE;
					bTaken = !(uOld & MASK_LOCKED);
					if(!bTaken){
						const bool bSpinFailureCountIncremented = uSpinFailureCount < SPIN_FAILURE_COUNT_MAX;
						uNew = uOld - THREADS_SPINNING_ONE + THREADS_TRAPPED_ONE + bSpinFailureCountIncremented * SPIN_FAILURE_COUNT_ONE;
					} else {
						const bool bSpinFailureCountDecremented = uSpinFailureCount != 0;
						uNew = uOld - THREADS_SPINNING_ONE + MASK_LOCKED - bSpinFailureCountDecremented * SPIN_FAILURE_COUNT_ONE;
					}
				} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
			}
			if(_MCFCRT_EXPECT(bTaken)){
				return true;
			}
		} else {
			{
				uintptr_t uOld, uNew;
				uOld = __atomic_load_n(puControl, __ATOMIC_RELAXED);
				do {
					bTaken = !(uOld & MASK_LOCKED);
					if(!bTaken){
						uNew = uOld + THREADS_TRAPPED_ONE;
					} else {
						uNew = uOld + MASK_LOCKED;
					}
				} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
			}
			if(_MCFCRT_EXPECT(bTaken)){
				return true;
			}
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
						bDecremented = uThreadsTrapped != 0;
						if(!bDecremented){
							break;
						}
						uNew = uOld - THREADS_TRAPPED_ONE;
					} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
				}
				if(bDecremented){
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
			bSignalOne = (uOld & MASK_THREADS_TRAPPED) != 0;
			uNew = (uOld & ~MASK_LOCKED) - bSignalOne * THREADS_TRAPPED_ONE;
		} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)));
	}
	// If `RtlDllShutdownInProgress()` is `true`, other threads will have been terminated.
	// Calling `NtReleaseKeyedEvent()` when no thread is waiting results in deadlocks. Don't do that.
	if(_MCFCRT_EXPECT_NOT(bSignalOne && !RtlDllShutdownInProgress())){
		NTSTATUS lStatus = NtReleaseKeyedEvent(_MCFCRT_NULLPTR, (void *)puControl, false, _MCFCRT_NULLPTR);
		_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtReleaseKeyedEvent() failed.");
		_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
	}
}

bool _MCFCRT_WaitForMutex(_MCFCRT_Mutex *pMutex, size_t uMaxSpinCount, uint64_t u64UntilFastMonoClock){
	const bool bLocked = ReallyWaitForMutex(&(pMutex->__u), uMaxSpinCount, true, u64UntilFastMonoClock);
	return bLocked;
}
void _MCFCRT_WaitForMutexForever(_MCFCRT_Mutex *pMutex, size_t uMaxSpinCount){
	const bool bLocked = ReallyWaitForMutex(&(pMutex->__u), uMaxSpinCount, false, UINT64_MAX);
	_MCFCRT_ASSERT(bLocked);
}
void _MCFCRT_SignalMutex(_MCFCRT_Mutex *pMutex){
	ReallySignalMutex(&(pMutex->__u));
}
