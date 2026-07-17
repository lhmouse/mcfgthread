/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/sem.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = { 0 };

int
main(void)
  {
    assert(_MCF_sem_get(&sem) == 0);

    assert(_MCF_sem_signal_some(&sem, _MCF_SEM_VALUE_MAX / 2) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX / 2);

    assert(_MCF_sem_signal_some(&sem, _MCF_SEM_VALUE_MAX / 2) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, _MCF_SEM_VALUE_MAX) == -2);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 1) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_signal_some(&sem, 1) == -2);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_wait(&sem, NULL) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 1) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_wait(&sem, NULL) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 2) == -2);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX - 1);
  }
