/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/sem.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = __MCF_0_INIT;

int
main(void)
  {
    assert(_MCF_sem_get(&sem) == 0);

    assert(_MCF_sem_signal_some(&sem, __MCF_SEM_VALUE_MAX / 2) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX / 2);

    assert(_MCF_sem_signal_some(&sem, __MCF_SEM_VALUE_MAX / 2) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, __MCF_SEM_VALUE_MAX) == -2);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 1) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_signal_some(&sem, 1) == -2);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_wait(&sem, __MCF_nullptr) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 1) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_wait(&sem, __MCF_nullptr) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 2) == -2);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);
  }
