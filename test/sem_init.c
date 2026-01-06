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

    assert(_MCF_sem_init(&sem, __MCF_SEM_VALUE_MAX) == 0);
    assert(_MCF_sem_get(&sem) == __MCF_SEM_VALUE_MAX);
  }
