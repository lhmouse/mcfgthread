/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_event event = __MCF_EVENT_INIT(3);

int
main(void)
  {
    assert(_MCF_event_get(&event) == 3);

    assert(_MCF_event_set(&event, 4) == 0);
    assert(_MCF_event_get(&event) == 4);

    assert(_MCF_event_set(&event, -1) == -1);
    assert(_MCF_event_get(&event) == 4);

    assert(_MCF_event_set(&event, __MCF_EVENT_VALUE_MAX + 1) == -1);
    assert(_MCF_event_get(&event) == 4);

    assert(_MCF_event_set(&event, __MCF_EVENT_VALUE_MAX) == 0);
    assert(_MCF_event_get(&event) == __MCF_EVENT_VALUE_MAX);
  }
