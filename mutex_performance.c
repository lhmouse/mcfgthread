/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>

#if defined USE_SRWLOCK
#  define my_mutex_t      SRWLOCK
#  define my_init(m)      InitializeSRWLock(m)
#  define my_lock(m)      AcquireSRWLockExclusive(m)
#  define my_unlock(m)    ReleaseSRWLockExclusive(m)
#elif defined USE_CRITICAL_SECTION
#  define my_mutex_t      CRITICAL_SECTION
#  define my_init(m)      InitializeCriticalSection(m)
#  define my_lock(m)      EnterCriticalSection(m)
#  define my_unlock(m)    LeaveCriticalSection(m)
#elif defined USE_WINPTHREAD
#  include <pthread.h>
#  define my_mutex_t      pthread_mutex_t
#  define my_init(m)      pthread_mutex_init(m, NULL)
#  define my_lock(m)      pthread_mutex_lock(m)
#  define my_unlock(m)    pthread_mutex_unlock(m)
#elif defined USE_MCFGTHREAD
#  include <mcfgthread/mutex.h>
#  define my_mutex_t      _MCF_mutex
#  define my_init(m)      _MCF_mutex_init(m)
#  define my_lock(m)      _MCF_mutex_lock_slow(m, NULL)
#  define my_unlock(m)    _MCF_mutex_unlock_slow(m)
#else
#  error No mutex type has been selected.
#endif

#if !defined NTHRD && !defined NITER
#  define NTHRD  16
#  define NITER  1000000
#endif

HANDLE start;
HANDLE threads[NTHRD];
my_mutex_t mutex;
volatile double dst = 1.2345;
volatile double src = 5.4321;
LARGE_INTEGER t0, t1, tf;

static
DWORD
__stdcall
thread_proc(void* arg)
  {
    (void) arg;
    WaitForSingleObject(start, INFINITE);

    for(intptr_t k = 0;  k < NITER;  ++k) {
      my_lock(&mutex);
      double temp = src;
      src = temp * 1.001;
      my_unlock(&mutex);

      temp = log(temp);

      my_lock(&mutex);
      dst = src;
      my_unlock(&mutex);

      SwitchToThread();
    }

    fprintf(stderr, "thread %d quitting\n", (int) GetCurrentThreadId());
    return 0;
  }

int
main(void)
  {
    start = CreateEventW(NULL, TRUE, FALSE, NULL);
    assert(start);

    my_init(&mutex);
    fprintf(stderr, "using `%s`:\n  # of threads    = %d\n  # of iterations = %d\n",
            _CRT_STRINGIZE(my_mutex_t), NTHRD, NITER);

    for(intptr_t k = 0;  k < NTHRD;  ++k) {
      threads[k] = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);
      assert(threads[k]);
    }

    fprintf(stderr, "main waiting\n");
    SetEvent(start);
    QueryPerformanceCounter(&t0);

    for(intptr_t k = 0;  k < NTHRD;  ++k) {
      WaitForSingleObject(threads[k], INFINITE);
      CloseHandle(threads[k]);
    }

    QueryPerformanceCounter(&t1);
    QueryPerformanceFrequency(&tf);
    fprintf(stderr, "total time:\n  %.3f milliseconds\n",
            (double) (t1.QuadPart - t0.QuadPart) * 1000 / tf.QuadPart);
  }
