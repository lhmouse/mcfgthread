/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#define __MCF_EXPAND_INLINE_DEFINITIONS  1
#include "../mcfgthread/once.h"
#include "../mcfgthread/thread.h"
#include "../mcfgthread/event.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_thread* reader;
static _MCF_thread* writer;

// reader_state
enum { rst_stopped, rst_starting, rst_running, rst_stopping };

struct datum
  {
    __MCF_ALIGNED(128) _MCF_once once;
    __MCF_ALIGNED(128) int value;
  };

static __MCF_ALIGNED(128) bool writer_exited = false;
static __MCF_ALIGNED(128) int reader_state = rst_stopped;
#define NDATA  16U
static struct datum shared_data[NDATA];

static
void
reader_proc(_MCF_thread* self)
  {
    while(_MCF_atomic_load_b_rlx(&writer_exited) == false) {
      // wait for writer
      while(_MCF_atomic_load_32_rlx(&reader_state) != rst_running) {
        _MCF_yield();
        _MCF_atomic_cmpxchg_32_rlx(&reader_state, &(int){ rst_starting }, rst_running);
        _MCF_atomic_cmpxchg_32_rlx(&reader_state, &(int){ rst_stopping }, rst_stopped);
      }

      for(uint32_t i = 0;  i < NDATA;  ++i) {
        // await initialization
        struct datum* p = shared_data + i;
        int* refptr = &(p->value);
        int err = _MCF_once_consume_wait(&(p->once), (void**) &refptr, &(int64_t){ 0 });
        assert(err <= 0);
        if(err == 0) {
          // consume
          assert(*refptr == 1);  // `assert(p->value == 1)` would fail on ARM64.
        }
      }
    }

    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

static
void
writer_proc(_MCF_thread* self)
  {
    int64_t start = _MCF_tick_count();
    while(_MCF_tick_count() < start + 5000) {
      // halt reader
      while(_MCF_atomic_load_32_rlx(&reader_state) != rst_stopped) {
        _MCF_yield();
        _MCF_atomic_cmpxchg_32_rlx(&reader_state, &(int){ rst_starting }, rst_stopping);
        _MCF_atomic_cmpxchg_32_rlx(&reader_state, &(int){ rst_running }, rst_stopping);
      }

      for(uint32_t i = 0;  i < NDATA;  ++i) {
        // re-initialize
        struct datum* p = shared_data + i;
        p->value = 0;
        _MCF_once_init(&(p->once));
        int err = _MCF_once_wait(&(p->once), NULL);
        assert(err == 1);
      }

      // start reader
      _MCF_atomic_store_32_rel(&reader_state, rst_starting);
      while(_MCF_atomic_load_32_rlx(&reader_state) != rst_running)
        _MCF_yield();

      for(uint32_t i = 0;  i < NDATA;  ++i) {
        // initialize
        struct datum* p = shared_data + i;
        p->value = 1;
        _MCF_once_release(&(p->once));
      }
    }

    _MCF_atomic_store_b_rlx(&writer_exited, true);

    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    reader = _MCF_thread_new(reader_proc, NULL, 0);
    assert(reader);
    writer = _MCF_thread_new(writer_proc, NULL, 0);
    assert(writer);

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(reader, NULL);
    _MCF_thread_wait(writer, NULL);

    fprintf(stderr, "main wait finished\n");
  }
