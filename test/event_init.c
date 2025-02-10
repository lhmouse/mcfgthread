/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static _MCF_event event = __MCF_EVENT_INIT(42);

int
main(void)
  {
    assert(_MCF_event_get(&event) == 42);

    assert(_MCF_event_init(&event, -1) == -1);
    assert(_MCF_event_get(&event) == 42);

    assert(_MCF_event_init(&event, 56) == 0);
    assert(_MCF_event_get(&event) == 56);

    assert(_MCF_event_init(&event, __MCF_EVENT_VALUE_MAX + 1) == -1);
    assert(_MCF_event_get(&event) == 56);

    assert(_MCF_event_init(&event, __MCF_EVENT_VALUE_MAX) == 0);
    assert(_MCF_event_get(&event) == __MCF_EVENT_VALUE_MAX);

    assert(_MCF_event_init(&event, 0) == 0);
    assert(_MCF_event_get(&event) == 0);
  }
