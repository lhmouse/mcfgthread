# Introduction to the MCF Gthread Mutex

### Synopsis

The `_MCF_mutex` type in MCF Gthread is a one-pointer-size structure with fields
like follows:

```c
typedef struct __MCF_mutex _MCF_mutex;

struct __MCF_mutex
  {
    uintptr_t __locked : 1;
    uintptr_t __sp_mask : 4;  /* mask of spinning threads; 1b/thread  */
    uintptr_t __sp_nfail : 4;  /* number of timeouts after spinning  */
    uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };
```

Initially all bits are zeroes, which means the mutex is not locked, and no thread
is spinning or waiting. In order to acquire/lock a mutex, a thread has to change
the `__locked` bit from zero to one. If it can't do so because the mutex has been
locked by another thread, then it must wait until the `__locked` bit becomes zero.

In an attempt to lock a mutex, a thread must test and update the mutex with _atomic
operations_, taking _exactly_ one of the three possible actions:

1. Change `__locked` from zero to one;
2. Change one bit of `__sp_mask` from zero to one;
3. Increment `__nsleep`.

If a thread takes _Action 1_, it will have locked the mutex. If a thread takes
_Action 3_, it will go to sleep on the global _Keyed Event_, passing the address
of the mutex as the key, so it can be woken up later. These are straightforward.

The most interesting and complex part is _Action 2_. If a thread has set a bit of
`__sp_mask`, it gains a chance to perform some busy-waiting.

### The Problems about Busy-waiting

A naive way of busy-waiting is to read and test the `__locked` bit repeatedly in
a loop. This is not incorrect, but it can increase pressure of the CPU bus a lot,
if there are a lot of threads running.

This can be addressed by allocating a separate flag byte for each spinning thread,
so the same location is not shared between CPU cores. An example is the [K42 lock
algorithm](https://locklessinc.com/articles/locks/), and **is also what Microsoft
has adopted in their `SRWLOCK`s**.

The fundamental problem about a linked list of waiters is that, it has to be
_reliable_. Because a thread that attempts to unlock a mutex must notify a next
waiter, it has to access this linked list. If a waiter could time out and remove
a node from the linked list, the notifier could access deallocated memory which
is catastrophic.

We have already noticed that a thread may stop spinning and go to sleep at any
time. So, is it possible to trade reliability for some efficiency?

### The Ultimate Solution

The ultimate solution by mcfgthread is to use a static hash table. A waiter is
assigned a flag byte from the table, according to the hash value of the mutex
and its bit index in `__sp_mask`. It then only needs to check its own flag byte.
If the byte is zero, it continues spinning; otherwise, it resets the byte to
zero and makes an attempt to lock the mutex. The flag bytes for all spinning
threads of the same mutex are scattered in the hash table such that they never
share the same cache line. The table will never be deallocated, so no notifier
can access deallocated memory.

The last question is, **what about hash collisions**? Well, we ignore collisions,
because they don't matter. This notification mechanism is not reliable, and never
has to be, as the number of spinning iterations is capped. A spinning thread will
use up its number of iterations sooner or later, and never will we get incorrect
results because of accidental hash collisions.
