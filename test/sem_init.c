/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/sem.h"
#include "../src/clock.h"
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = __MCF_SEM_INIT(42);

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
  }
