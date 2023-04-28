/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_CXX11_
#define __MCFGTHREAD_CXX11_

#include "fwd.h"
#include "gthr_aux.h"
#include "cxa.h"
#include "event.h"
#include <chrono>  // duration, time_point
#include <functional>  // mem_fn(), invoke()
#include <system_error>  // system_error, errc, error_code
#include <mutex>  // unique_lock, lock_guard
#include <new>  // operator new()
#include <type_traits>  // many
#include <iosfwd>

namespace _MCF {

// Forward declarations
namespace _Noadl = ::_MCF;
extern "C" void* __dso_handle;

namespace chrono = ::std::chrono;
using ::std::lock_guard;
using ::std::unique_lock;
#ifdef __cpp_lib_scoped_lock
using ::std::scoped_lock;
#endif
using ::std::defer_lock_t;
using ::std::defer_lock;
using ::std::try_to_lock_t;
using ::std::try_to_lock;
using ::std::adopt_lock_t;
using ::std::adopt_lock;
using ::std::try_lock;
using ::std::lock;

struct once_flag;
class mutex;
using timed_mutex = mutex;
class recursive_mutex;
using recursive_timed_mutex = recursive_mutex;
enum class cv_status { no_timeout, timeout };
class condition_variable;
class thread;

template<typename _Tp>
class thread_specific_ptr;  // inspired by boost

// Provide limited support for `-fno-exceptions`.
#if defined(__EXCEPTIONS) || defined(__cpp_exceptions)

#  define __MCF_TRY          try
#  define __MCF_CATCH(...)   catch(__VA_ARGS__)
#  define __MCF_THROW(...)   throw __VA_ARGS__

#  define __MCF_THROW_SYSTEM_ERROR(_Code, _Msg)  \
     throw ::std::system_error((int) (::std::errc::_Code), ::std::generic_category(), _Msg)

#else  // __EXCEPTIONS

#  define __MCF_TRY          if(true)
#  define __MCF_CATCH(...)   else if(false)
#  define __MCF_THROW(...)   ::std::terminate()

#  define __MCF_THROW_SYSTEM_ERROR(_Code, _Msg)  \
     ::__MCF_runtime_failure(_Msg)

#endif  // __EXCEPTIONS

// Provide `INVOKE` for C++11. [func.require]
// At the moment, all results are discarded, so the expression always has a
// type of `void` for simplicity.
#if defined(__cpp_lib_invoke)

#  define __MCF_VOID_INVOKE(_F, ...)   ((void) ::std::invoke(_F, __VA_ARGS__))

#else  // std::invoke

template<class _Member, class _Class, class... _Args>
void
__void_invoke(_Member _Class::* __memp, _Args&&... __args)
  {
    ::std::mem_fn(__memp) (::std::forward<_Args>(__args)...);
  }

template<class _Callable, class... _Args>
void
__void_invoke(_Callable&& __callable, _Args&&... __args)
  {
    ::std::forward<_Callable>(__callable) (::std::forward<_Args>(__args)...);
  }

#  define __MCF_VOID_INVOKE(_F, ...)   (::_MCF::__void_invoke(_F, __VA_ARGS__))

#endif  // std::invoke

// Define shortcuts for `enable_if`.
#define __MCF_SFINAE_ENABLE_IF(...)   typename ::std::enable_if<(bool) (__VA_ARGS__)>::type* = nullptr
#define __MCF_SFINAE_DISABLE_IF(...)  typename ::std::enable_if<!(bool) (__VA_ARGS__)>::type* = nullptr

// This is the maximum integer representable as a `double` exact.
constexpr int64_t _Max_ms = 0x7FFFFFFFFFFFFC00;

// Convert a `chrono::duration` to the number of milliseconds, represented
// as a non-negative 64-bit integer. For everything other than milliseconds,
// it is necessary to perform calculation using a floating-point type and
// check for overflows before casting.
__MCF_CXX14(constexpr) inline
int64_t
__clamp_duration(const chrono::milliseconds& __ms) noexcept
  {
    if(__ms.count() < 0)
      return 0;
    else if(__ms.count() > _Max_ms)
      return _Max_ms;
    else
      return __ms.count();
  }

template<typename _Duration>
__MCF_CXX14(constexpr) inline
int64_t
__clamp_duration(const _Duration& __dur)
  {
    chrono::duration<double, ::std::milli> __ms(__dur);
    if(__ms.count() != __ms.count())
      __MCF_THROW_SYSTEM_ERROR(argument_out_of_domain, "chrono::duration: argument was NaN");
    else if(__ms.count() < 0)
      return 0;
    else if(__ms.count() > _Max_ms)
      return _Max_ms;
    else
      return (int64_t) (__ms.count() + 0.9999999);
  }

// Suspend the current thread until the given timeout. The operation is
// noninterruptible, and this function always returns zero.
inline
int
__sleep_noninterruptible(int64_t* __timeout_opt) noexcept
  {
    ::_MCF_sleep_noninterruptible(__timeout_opt);
    return 0;
  }

// Wait an amount of time, or until a time point. The condition function
// shall return `int`. If it returns zero, this function also returns zero;
// otherwise it is called repeatedly, until the timeout has been reached,
// and this function returns its last value.
template<typename _Rep, typename _Period, typename _Cond, typename... _Args>
inline
int
__wait_for(const chrono::duration<_Rep, _Period>& __rel_time, _Cond&& __cond, _Args&&... __args)
  {
    int64_t __timeout = _Noadl::__clamp_duration(__rel_time);
    __timeout *= -1;
    return __cond(__args..., &__timeout);
  }

template<typename _Dur, typename _Cond, typename... _Args>
inline
int
__wait_until(const chrono::time_point<chrono::system_clock, _Dur>& __abs_time, _Cond&& __cond, _Args&&... __args)
  {
    int64_t __timeout = _Noadl::__clamp_duration(__abs_time.time_since_epoch());
    return __cond(__args..., &__timeout);
  }

template<typename _Clock, typename _Dur, typename _Cond, typename... _Args>
inline
int
__wait_until(const chrono::time_point<_Clock, _Dur>& __abs_time, _Cond&& __cond, _Args&&... __args)
  {
    int __err;
    int64_t __timeout;

    do {
      __timeout = _Noadl::__clamp_duration(__abs_time - _Clock::now());
      __timeout *= -1;
      __err = __cond(__args..., &__timeout);
    }
    while((__err < 0) && (__timeout != 0));  // loop if timed out

    return __err;
  }

// Registers a callback upon the current thread's exit.
template<typename _Result, typename _Value>
inline
void
__check_thread_atexit(_Result __target(_Value*), typename ::std::common_type<_Value*>::type __ptr)
  {
    static_assert(::std::is_scalar<_Result>::value || ::std::is_void<_Result>::value, "result not discardable");

    int __err = ::__MCF_cxa_thread_atexit((__MCF_cxa_dtor_cdecl*)(intptr_t) __target, __ptr, &__dso_handle);
    if(__err != 0)
      __MCF_THROW_SYSTEM_ERROR(not_enough_memory, "__MCF_cxa_thread_atexit");
  }

// Create a thread. ISO C++ requires that arguments for the constructor of
// `std::thread` be passed as decay-copied rvalues. This is the object that
// saves copies of them and invokes the target function accordingly.
template<typename _Callable, typename... _Args>
struct _Invoke_decay_copy;

template<typename _Callable>
struct _Invoke_decay_copy<_Callable>
  {
    typename ::std::decay<_Callable>::type _M_callable;

    explicit
    _Invoke_decay_copy(_Callable& __callable)
      : _M_callable(::std::forward<_Callable>(__callable))
      {
      }

    template<typename... _Args>
    void
    __do_it(_Args&... __args)
      {
        __MCF_VOID_INVOKE(::std::move(this->_M_callable), ::std::move(__args)...);
      }
  };

template<typename _Callable, typename _Mine, typename... _Others>
struct _Invoke_decay_copy<_Callable, _Mine, _Others...>
  {
    typename ::std::decay<_Mine>::type _M_mine;
    _Invoke_decay_copy<_Callable, _Others...> _M_next;

    explicit
    _Invoke_decay_copy(_Callable& __callable, _Mine& __mine, _Others&... __others)
      : _M_mine(::std::forward<_Mine>(__mine)), _M_next(__callable, __others...)
      {
      }

    template<typename... _Args>
    void
    __do_it(_Args&... __args)
      {
        this->_M_next.__do_it(__args..., this->_M_mine);
      }
  };

// Reference implementation for [thread.once.onceflag]
struct once_flag
  {
    ::_MCF_once _M_once[1] = { };

    once_flag(const once_flag&) = delete;
    once_flag& operator=(const once_flag&) = delete;

    constexpr once_flag() noexcept { }
  };

// Reference implementation for [thread.once.callonce]
template<typename _Callable, typename... _Args>
void
call_once(once_flag& __flag, _Callable&& __callable, _Args&&... __args)
  {
    int __err = ::_MCF_once_wait(__flag._M_once, nullptr);
    if(__err == 0)
      return;  // passive

    __MCF_ASSERT(__err == 1);

    __MCF_TRY {
      // active
      __MCF_VOID_INVOKE(::std::forward<_Callable>(__callable), ::std::forward<_Args>(__args)...);
    }
    __MCF_CATCH(...) {
      // exceptional
      ::_MCF_once_abort(__flag._M_once);
      __MCF_THROW();
    }

    // returning
    ::_MCF_once_release(__flag._M_once);
  }

// Reference implementation for [thread.mutex.class] and
// [thread.timedmutex.class].
class mutex
  {
  private:
    ::_MCF_mutex _M_mtx[1] = { };

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

  public:
    constexpr mutex() noexcept { }

    using native_handle_type = ::_MCF_mutex*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept
      {
        return this->_M_mtx;
      }

    void
    lock() noexcept  // strengthened
      {
        int __err = ::_MCF_mutex_lock(this->_M_mtx, nullptr);
        __MCF_ASSERT(__err == 0);
      }

    bool
    try_lock() noexcept  // strengthened
      {
        int64_t __timeout = 0;
        int __err = ::_MCF_mutex_lock(this->_M_mtx, &__timeout);
        return __err == 0;
      }

    template<typename _Clock, typename _Dur>
    bool
    try_lock_until(const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        int __err = _Noadl::__wait_until(__abs_time, ::_MCF_mutex_lock, this->_M_mtx);
        return __err == 0;
      }

    template<typename _Rep, typename _Period>
    bool
    try_lock_for(const chrono::duration<_Rep, _Period>& __rel_time)
      {
        int __err = _Noadl::__wait_for(__rel_time, ::_MCF_mutex_lock, this->_M_mtx);
        return __err == 0;
      }

    void
    unlock() noexcept  // strengthened
      {
        ::_MCF_mutex_unlock(this->_M_mtx);
      }
  };

// Reference implementation for [thread.timedmutex.recursive] and
// [thread.timedmutex.recursive].
class recursive_mutex
  {
  private:
    ::__MCF_gthr_rc_mutex _M_rmtx[1] = { };

    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;

  public:
    constexpr recursive_mutex() noexcept { }  // strengthened

    using native_handle_type = ::__MCF_gthr_rc_mutex*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept
      {
        return this->_M_rmtx;
      }

    void
    lock() noexcept  // strengthened
      {
        int __err = ::__MCF_gthr_rc_mutex_recurse(this->_M_rmtx);
        if(__err != 0)
          __err = ::__MCF_gthr_rc_mutex_wait(this->_M_rmtx, nullptr);
        __MCF_ASSERT(__err == 0);
      }

    bool
    try_lock() noexcept  // strengthened
      {
        int64_t __timeout = 0;
        int __err = ::__MCF_gthr_rc_mutex_recurse(this->_M_rmtx);
        if(__err != 0)
          __err = ::__MCF_gthr_rc_mutex_wait(this->_M_rmtx, &__timeout);
        return __err == 0;
      }

    template<typename _Clock, typename _Dur>
    bool
    try_lock_until(const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        int __err = ::__MCF_gthr_rc_mutex_recurse(this->_M_rmtx);
        if(__err != 0)
          __err = _Noadl::__wait_until(__abs_time, ::__MCF_gthr_rc_mutex_wait, this->_M_rmtx);
        return __err == 0;
      }

    template<typename _Rep, typename _Period>
    bool
    try_lock_for(const chrono::duration<_Rep, _Period>& __rel_time)
      {
        int __err = ::__MCF_gthr_rc_mutex_recurse(this->_M_rmtx);
        if(__err != 0)
          __err = _Noadl::__wait_for(__rel_time, ::__MCF_gthr_rc_mutex_wait, this->_M_rmtx);
        return __err == 0;
      }

    void
    unlock() noexcept  // strengthened
      {
        ::__MCF_gthr_rc_mutex_release(this->_M_rmtx);
      }
  };

// Reference implementation for [thread.condition.condvar]
class condition_variable
  {
  private:
    ::_MCF_cond _M_cnd[1] = { };

    condition_variable(const condition_variable&) = delete;
    condition_variable& operator=(const condition_variable&) = delete;

  public:
    constexpr condition_variable() noexcept { }  // strengthened

    using native_handle_type = ::_MCF_cond*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept
      {
        return this->_M_cnd;
      }

    void
    notify_one() noexcept
      {
        ::_MCF_cond_signal(this->_M_cnd);
      }

    void
    notify_all() noexcept
      {
        ::_MCF_cond_signal_all(this->_M_cnd);
      }

    void
    wait(unique_lock<mutex>& __lock) noexcept  // strengthened
      {
        __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock<mutex> __temp_lock;
        __temp_lock.swap(__lock);

        int __err = ::_MCF_cond_wait(this->_M_cnd,
             ::__MCF_gthr_mutex_unlock_callback, ::__MCF_gthr_mutex_relock_callback,
             (intptr_t) __temp_lock.mutex()->native_handle(), nullptr);

        __temp_lock.swap(__lock);
        __MCF_ASSERT(__err == 0);
      }

    template<typename _Predicate>
    void
    wait(unique_lock<mutex>& __lock, _Predicate&& __pred)
      {
        while(!(bool) __pred())
          this->wait(__lock);
      }

    template<typename _Clock, typename _Dur>
    cv_status
    wait_until(unique_lock<mutex>& __lock, const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock<mutex> __temp_lock;
        __temp_lock.swap(__lock);

        int __err = _Noadl::__wait_until(__abs_time, ::_MCF_cond_wait, this->_M_cnd,
             ::__MCF_gthr_mutex_unlock_callback, ::__MCF_gthr_mutex_relock_callback,
             (intptr_t) __temp_lock.mutex()->native_handle());

        __temp_lock.swap(__lock);
        return (__err == 0) ? cv_status::no_timeout : cv_status::timeout;
      }

    template<typename _Clock, typename _Dur, typename _Predicate>
    bool
    wait_until(unique_lock<mutex>& __lock, const chrono::time_point<_Clock, _Dur>& __abs_time, _Predicate&& __pred)
      {
        while(!(bool) __pred())
          if(this->wait_until(__lock, __abs_time) == cv_status::timeout)
            return __pred();
        return true;
      }

    template<typename _Rep, typename _Period>
    cv_status
    wait_for(unique_lock<mutex>& __lock, const chrono::duration<_Rep, _Period>& __rel_time)
      {
        __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock<mutex> __temp_lock;
        __temp_lock.swap(__lock);

        int __err = _Noadl::__wait_for(__rel_time, ::_MCF_cond_wait, this->_M_cnd,
             ::__MCF_gthr_mutex_unlock_callback, ::__MCF_gthr_mutex_relock_callback,
             (intptr_t) __temp_lock.mutex()->native_handle());

        __temp_lock.swap(__lock);
        return (__err == 0) ? cv_status::no_timeout : cv_status::timeout;
      }

    template<typename _Rep, typename _Period, typename _Predicate>
    bool
    wait_for(unique_lock<mutex>& __lock, const chrono::duration<_Rep, _Period>& __rel_time, _Predicate&& __pred)
      {
        return this->wait_until(__lock, chrono::steady_clock::now() + __rel_time, __pred);
      }
  };

// Reference implementation for [thread.condition.nonmember]
inline
void
notify_all_at_thread_exit(condition_variable& __cond, unique_lock<mutex> __lock)
  {
    __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
    __MCF_ASSERT(__lock.mutex() != nullptr);

    _Noadl::__check_thread_atexit(::_MCF_cond_signal_all, __cond.native_handle());
    _Noadl::__check_thread_atexit(::_MCF_mutex_unlock, __lock.mutex()->native_handle());
    __lock.release();
  }

// Reference implementation for [thread.thread.class]
class thread
  {
  private:
    ::_MCF_thread* _M_thr = nullptr;

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

  public:
    constexpr thread() noexcept { }

    template<typename _Callable, typename... _Args,
    __MCF_SFINAE_DISABLE_IF(::std::is_same<typename ::std::decay<_Callable>::type, thread>::value)>
    explicit
    thread(_Callable&& __callable, _Args&&... __args)
      {
        using _My_invoker = _Invoke_decay_copy<_Callable, _Args...>;
        enum { _St_zero, _St_constructed, _St_cancelled };

        struct _My_data
          {
            _My_invoker _M_invoker[1];
            ::_MCF_event _M_status[1];

            static
            void
            __do_it(::_MCF_thread* __thr)
              {
                _My_data* const __my = (_My_data*) ::_MCF_thread_get_data(__thr);

                // Check whether `*_M_invoker` has been constructed. If its
                // constructor failed, this thread shall exit immediately.
                int __st = ::_MCF_event_await_change(__my->_M_status, _St_zero, nullptr);
                if(__st == _St_cancelled)
                  return;

                // Execute the user-defined procedure.
                __MCF_ASSERT(__st == _St_constructed);
                __my->_M_invoker->__do_it();
                __my->_M_invoker->~_My_invoker();
              };
          };

        // Create the thread. User-defined data are initialized to zeroes.
        this->_M_thr = ::_MCF_thread_new_aligned(_My_data::__do_it, alignof(_My_data), nullptr, sizeof(_My_data));
        if(!this->_M_thr)
          __MCF_THROW_SYSTEM_ERROR(resource_unavailable_try_again, "_MCF_thread_new_aligned");

        _My_data* const __my = (_My_data*) ::_MCF_thread_get_data(this->_M_thr);

        __MCF_TRY {
          // Construct `_M_invoker`.
          ::new((void*) __my->_M_invoker) _My_invoker(__callable, __args...);
        }
        __MCF_CATCH(...) {
          // Cancel the thread.
          ::_MCF_event_set(__my->_M_status, _St_cancelled);
          ::_MCF_thread_drop_ref(this->_M_thr);
          __MCF_THROW();
        }

        // Let the thread go.
        ::_MCF_event_set(__my->_M_status, _St_constructed);
      }

    thread(thread&& __other) noexcept
      {
        this->_M_thr = __other._M_thr;
        __other._M_thr = nullptr;
      }

    thread&
    operator=(thread&& __other) noexcept
      {
        if(this->_M_thr)
          ::std::terminate();

        this->_M_thr = __other._M_thr;
        __other._M_thr = nullptr;

        return *this;
      }

    ~thread() noexcept
      {
        if(this->_M_thr)
          ::std::terminate();
      }

    using native_handle_type = ::_MCF_thread*;

    struct id
      {
        uint32_t _M_tid = 0;  // native thread ID

        constexpr id() noexcept { }
      };

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept
      {
        return this->_M_thr;
      }

    void
    swap(thread& __other) noexcept
      {
        ::std::swap(this->_M_thr, __other._M_thr);
      }

    bool
    joinable() const noexcept
      {
        return this->_M_thr != nullptr;
      }

    void
    join()
      {
        __MCF_ASSERT(this->_M_thr);  // must be joinable

        if(::_MCF_thread_get_tid(this->_M_thr) == ::_MCF_thread_self_tid())
          __MCF_THROW_SYSTEM_ERROR(resource_deadlock_would_occur, "thread::join");

        int __err = ::_MCF_thread_wait(this->_M_thr, nullptr);
        __MCF_ASSERT(__err == 0);

        ::_MCF_thread_drop_ref(this->_M_thr);
        this->_M_thr = nullptr;
      }

    void
    detach() noexcept  // strengthened
      {
        __MCF_ASSERT(this->_M_thr);  // must be joinable

        ::_MCF_thread_drop_ref(this->_M_thr);
        this->_M_thr = nullptr;
      }

    __MCF_CXX14(constexpr)
    id
    get_id() const noexcept
      {
        id __id;
        if(this->_M_thr)
          __id._M_tid = ::_MCF_thread_get_tid(this->_M_thr);
        return __id;
      }

    static
    unsigned
    hardware_concurrency() noexcept
      {
        return (unsigned) ::_MCF_get_processor_count();
      }
  };

inline
void
swap(thread& __lhs, thread& __rhs) noexcept
  {
    __lhs.swap(__rhs);
  }

constexpr
bool
operator==(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid == __y._M_tid;
  }

#ifdef __cpp_lib_three_way_comparison

constexpr
::std::strong_ordering
operator<=>(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid <=> __y._M_tid;
  }

#else  // __cpp_lib_three_way_comparison

constexpr
bool
operator!=(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid != __y._M_tid;
  }

constexpr
bool
operator<(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid < __y._M_tid;
  }

constexpr
bool
operator>(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid > __y._M_tid;
  }

constexpr
bool
operator<=(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid <= __y._M_tid;
  }

constexpr
bool
operator>=(thread::id __x, thread::id __y) noexcept
  {
    return __x._M_tid >= __y._M_tid;
  }

#endif  // __cpp_lib_three_way_comparison

template<typename _CharT, typename _Traits>
inline
::std::basic_ostream<_CharT, _Traits>&
operator<<(::std::basic_ostream<_CharT, _Traits>& __os, thread::id __id)
  {
    return __os << __id._M_tid;
  }

namespace this_thread
  {
    inline
    thread::id
    get_id() noexcept
      {
        thread::id __id;
        __id._M_tid = ::_MCF_thread_self_tid();
        return __id;
      }

    inline
    void
    yield() noexcept  // strengthened
      {
        ::_MCF_yield();
      }

    template<typename _Clock, typename _Dur>
    inline
    void
    sleep_until(const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        _Noadl::__wait_until(__abs_time, _Noadl::__sleep_noninterruptible);
      }

    template<typename _Rep, typename _Period>
    inline
    void
    sleep_for(const chrono::duration<_Rep, _Period>& __rel_time)
      {
        _Noadl::__wait_for(__rel_time, _Noadl::__sleep_noninterruptible);
      }
  }

template<typename _Tp>
class thread_specific_ptr
  {
  private:
    ::_MCF_tls_key* _M_key = nullptr;

    thread_specific_ptr(const thread_specific_ptr&) = delete;
    thread_specific_ptr& operator=(const thread_specific_ptr&) = delete;

    static
    void
    __default_cleanup(_Tp* __ptr) noexcept
      {
        delete __ptr;
      }

    using _Native_cleanup = void (void*);
    using _Specialized_cleanup = void (_Tp*);

  public:
    explicit
    thread_specific_ptr(_Specialized_cleanup* __cleanup_opt)
      {
        this->_M_key = ::_MCF_tls_key_new((_Native_cleanup*) __cleanup_opt);
        if(!this->_M_key)
          __MCF_THROW_SYSTEM_ERROR(resource_unavailable_try_again, "_MCF_tls_key_new");
      }

    thread_specific_ptr()
      : thread_specific_ptr(__default_cleanup)
      {
      }

    ~thread_specific_ptr()
      {
        ::_MCF_tls_key_delete(this->_M_key);
      }

    using native_handle_type = ::_MCF_tls_key*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept
      {
        return this->_M_key;
      }

    using element_type = _Tp;
    using pointer = _Tp*;

    pointer
    get() const noexcept
      {
        return (pointer) ::_MCF_tls_get(this->_M_key);
      }

    explicit
    operator bool() const noexcept
      {
        return ::_MCF_tls_get(this->_M_key) != nullptr;
      }

    typename ::std::add_lvalue_reference<element_type>::type
    operator*() const noexcept
      {
        void* __ptr = ::_MCF_tls_get(this->_M_key);
        __MCF_ASSERT(__ptr);
        return *(pointer) __ptr;
      }

    pointer
    operator->() const noexcept
      {
        void* __ptr = ::_MCF_tls_get(this->_M_key);
        __MCF_ASSERT(__ptr);
        return (pointer) __ptr;
      }

    void
    reset(pointer __ptr = nullptr)
      {
        void* __ptr_old = nullptr;
        int __err = ::_MCF_tls_xset(this->_M_key, &__ptr_old, __ptr);
        if(__err < 0)
          __MCF_THROW_SYSTEM_ERROR(not_enough_memory, "_MCF_tls_xset");

        if(__ptr_old)
          if(auto __my_cleanup = ::_MCF_tls_key_get_destructor(this->_M_key))
            __my_cleanup(__ptr_old);
      }

    pointer
    release() noexcept
      {
        void* __ptr_old = nullptr;
        ::_MCF_tls_xset(this->_M_key, &__ptr_old, nullptr);
        return (pointer) __ptr_old;
      }
  };

}  /* namespace MCF  */
#endif  /* __MCFGTHREAD_CXX11_  */
