/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/sem.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = __MCF_SEM_INIT(3);

int
main(void)
  {
    assert(_MCF_sem_get(&sem) == 3);

    assert(_MCF_sem_wait(&sem, (const int64_t[]){0}) == 0);
    assert(_MCF_sem_get(&sem) == 2);

    assert(_MCF_sem_wait(&sem, (const int64_t[]){0}) == 0);
    assert(_MCF_sem_get(&sem) == 1);

    assert(_MCF_sem_wait(&sem, (const int64_t[]){0}) == 0);
    assert(_MCF_sem_get(&sem) == 0);

    assert(_MCF_sem_wait(&sem, (const int64_t[]){0}) == -1);
    assert(_MCF_sem_get(&sem) == 0);

    assert(_MCF_sem_signal_some(&sem, 2) == 0);
    assert(_MCF_sem_get(&sem) == 2);

    assert(_MCF_sem_wait(&sem, (const int64_t[]){0}) == 0);
    assert(_MCF_sem_get(&sem) == 1);

    assert(_MCF_sem_wait(&sem, (const int64_t[]){0}) == 0);
    assert(_MCF_sem_get(&sem) == 0);
  }
