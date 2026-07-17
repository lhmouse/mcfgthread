/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/sem.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = _MCF_SEM_INIT(42);

int
main(void)
  {
    assert(_MCF_sem_get(&sem) == 42);

    assert(_MCF_sem_init(&sem, -1) == -1);
    assert(_MCF_sem_get(&sem) == 42);

    assert(_MCF_sem_init(&sem, 56) == 0);
    assert(_MCF_sem_get(&sem) == 56);

    assert(_MCF_sem_init(&sem, -100) == -1);
    assert(_MCF_sem_get(&sem) == 56);

    assert(_MCF_sem_init(&sem, 0) == 0);
    assert(_MCF_sem_get(&sem) == 0);

    assert(_MCF_sem_init(&sem, _MCF_SEM_VALUE_MAX) == 0);
    assert(_MCF_sem_get(&sem) == _MCF_SEM_VALUE_MAX);
  }
