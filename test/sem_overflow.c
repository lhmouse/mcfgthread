/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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

    assert(_MCF_sem_wait(&sem, NULL) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 1) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX);

    assert(_MCF_sem_wait(&sem, NULL) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);

    assert(_MCF_sem_signal_some(&sem, 2) == -2);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX - 1);
  }
