// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <mcfgthread/gthr.h>
#include <mcfgthread/clock.h>
#include <pthread.h>


#if defined(USE_SRWLOCK)

#  define my_mutex_t      SRWLOCK
#  define my_init(m)      InitializeSRWLock(m)
#  define my_lock(m)      AcquireSRWLockExclusive(m)
#  define my_unlock(m)    ReleaseSRWLockExclusive(m)

#elif defined(USE_CRITICAL_SECTION)

#  define my_mutex_t      CRITICAL_SECTION
#  define my_init(m)      InitializeCriticalSection(m)
#  define my_lock(m)      EnterCriticalSection(m)
#  define my_unlock(m)    LeaveCriticalSection(m)

#elif defined(USE_WINPTHREAD)

#  define my_mutex_t      pthread_mutex_t
#  define my_init(m)      pthread_mutex_init(m, NULL)
#  define my_lock(m)      pthread_mutex_lock(m)
#  define my_unlock(m)    pthread_mutex_unlock(m)

#elif defined(USE_MCFGTHREAD)

#  define my_mutex_t      __gthread_mutex_t
#  define my_init(m)      __GTHREAD_MUTEX_INIT_FUNCTION(m)
#  define my_lock(m)      __gthread_mutex_lock(m)
#  define my_unlock(m)    __gthread_mutex_unlock(m)

#else

#  error No mutex type has been selected.

#endif


#define NTHRD  16
#define NITER  1000000

HANDLE event;
__gthread_t threads[NTHRD];
my_mutex_t mutex;
volatile double dst = 12345;
volatile double src = 54321;

static
void*
thread_proc(void* arg)
  {
    (void) arg;
    WaitForSingleObject(event, INFINITE);

    for(intptr_t k = 0;  k < NITER;  ++k) {
      my_lock(&mutex);
      double temp = src;
      my_unlock(&mutex);

      temp = log(temp);

      my_lock(&mutex);
      dst = src;
      my_unlock(&mutex);

      SwitchToThread();
    }

    //printf("thread %d quitting\n", (int) GetCurrentThreadId());
    return NULL;
  }

int
main(void)
  {
#define xstr1(x)  xstr2(x)
#define xstr2(x)  #x
    printf("using `%s`:\n  # of threads    = %d\n  # of iterations = %d\n",
           xstr1(my_mutex_t), NTHRD, NITER);

    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    __MCFGTHREAD_CHECK(event);

    my_init(&mutex);

    for(intptr_t k = 0;  k < NTHRD;  ++k)
      __MCFGTHREAD_CHECK(__gthread_create(&threads[k], thread_proc, NULL) == 0);

    printf("main waiting\n");
    SetEvent(event);
    double t_sta = _MCF_perf_counter();

    for(intptr_t k = 0;  k < NTHRD;  ++k)
      __MCFGTHREAD_CHECK(__gthread_join(threads[k], NULL) == 0);

    double t_fin = _MCF_perf_counter();
    printf("total time:\n  %.3f milliseconds\n", t_fin - t_sta);
  }
