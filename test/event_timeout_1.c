/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_event event = _MCF_EVENT_INIT(42);

int
main(void)
  {
    int r = _MCF_event_await_change(&event, -1,
        &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);

    r = _MCF_event_await_change(&event, _MCF_EVENT_VALUE_MAX + 1,
        &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 42);
  }
