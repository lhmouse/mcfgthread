/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include <assert.h>
#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include <timeapi.h>
#include <pthread.h>
#include <mcfgthread/mutex.h>

#if defined USE_SRWLOCK
#  define my_mutex_name   "SRWLOCK"
#  define my_mutex_t      SRWLOCK
#  define my_init(m)      InitializeSRWLock(m)
#  define my_lock(m)      AcquireSRWLockExclusive(m)
#  define my_unlock(m)    ReleaseSRWLockExclusive(m)
#elif defined USE_CRITICAL_SECTION
#  define my_mutex_name   "CRITICAL_SECTION"
#  define my_mutex_t      CRITICAL_SECTION
#  define my_init(m)      InitializeCriticalSection(m)
#  define my_lock(m)      EnterCriticalSection(m)
#  define my_unlock(m)    LeaveCriticalSection(m)
#elif defined USE_WINPTHREAD
#  define my_mutex_name   "WINPTHREAD"
#  define my_mutex_t      pthread_mutex_t
#  define my_init(m)      pthread_mutex_init(m, NULL)
#  define my_lock(m)      pthread_mutex_lock(m)
#  define my_unlock(m)    pthread_mutex_unlock(m)
#elif defined USE_MCFGTHREAD
#  define my_mutex_name   "MCFGTHREAD"
#  define my_mutex_t      _MCF_mutex
#  define my_init(m)      _MCF_mutex_init(m)
#  define my_lock(m)      _MCF_mutex_lock(m, NULL)
#  define my_unlock(m)    _MCF_mutex_unlock(m)
#else
#  error No mutex type has been selected.
#endif

#if !defined NTHRD
#  define NTHRD  16
#endif

#if !defined MS_PER_THRD
#  define MS_PER_THRD  2000
#endif

struct thread_record
  {
    alignas(64) HANDLE handle;
    int64_t t0, t1, niter;
  };

struct shared_state
  {
    my_mutex_t mutex[1];
    uint64_t lcg;
  };

HANDLE start;
struct thread_record records[NTHRD];
struct shared_state st;

static
DWORD
__stdcall
thread_proc(void* arg)
  {
    WaitForSingleObject(start, INFINITE);

    // calibrate
    const size_t k = (size_t) arg;
    records[k].t1 = GetTickCount64();

    do {
      SwitchToThread();
      records[k].t0 = GetTickCount64();
    } while(records[k].t1 == records[k].t0);

    do {
      my_lock(st.mutex);
      for(intptr_t k = 0;  k < 50;  ++k) {
        // This is the implementation of `drand48()` of GNU libc.
        st.lcg *= 0x5DEECE66D;
        st.lcg += 0xB;
        st.lcg &= 0xFFFFFFFFFFFF;
        __asm__ volatile ("" : "+m"(st.lcg));
      }
      my_unlock(st.mutex);

      YieldProcessor();

      records[k].t1 = GetTickCount64();
      records[k].niter ++;
    } while((records[k].t1 - records[k].t0 < MS_PER_THRD)
            || (records[k].niter < 1000));

    //fprintf(stderr, "thread %d quitting\n", (int) GetCurrentThreadId());
    return 0;
  }

int
main(void)
  {
    fprintf(stderr, "benchmarking %s, NTHR %d\n", my_mutex_name, NTHRD);
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    timeBeginPeriod(1);

    start = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(start);
    my_init(st.mutex);

    for(intptr_t k = 0;  k < NTHRD;  ++k) {
      records[k].handle = CreateThread(NULL, 0, thread_proc, (void*) k, 0, NULL);
      assert(records[k].handle);
      SetThreadPriority(records[k].handle, THREAD_PRIORITY_TIME_CRITICAL);
    }

    SetEvent(start);

    double total_ns = 0;
    double total_niter = 0;

    for(intptr_t k = 0;  k < NTHRD;  ++k) {
      WaitForSingleObject(records[k].handle, INFINITE);
      CloseHandle(records[k].handle);
      total_ns += (records[k].t1 - records[k].t0) * 1.0e6;
      total_niter += records[k].niter;
    }

    double ns_per_iteration = total_ns / total_niter;
    fprintf(stderr, "=> %.3f ns / iteration\n", ns_per_iteration);

    printf("%s\t%d\t%.3f\n", my_mutex_name, NTHRD, ns_per_iteration);
    fflush(NULL);
    _exit(0);
  }
