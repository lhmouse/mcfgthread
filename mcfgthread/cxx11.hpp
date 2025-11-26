/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_CXX11_
#define __MCFGTHREAD_CXX11_

#include "fwd.h"
#include "gthr_aux.h"
#include "cxa.h"
#include "event.h"
#include "shared_mutex.h"
#include <chrono>  // duration, time_point
#include <functional>  // mem_fn(), invoke()
#include <system_error>  // system_error, errc, error_code
#include <mutex>  // unique_lock, lock_guard
#include <new>  // operator new()
#include <iosfwd>  // basic_ostream
#include <type_traits>  // many
#if __MCF_CXX14(1+)0
#include <shared_mutex>  // shared_lock
#endif

namespace _MCF {

// Forward declarations
namespace _Noadl = ::_MCF;
extern "C" void* __dso_handle;

namespace chrono = ::std::chrono;
using ::std::lock_guard;
using ::std::unique_lock;
#if __MCF_CXX14(1+)0
using ::std::shared_lock;
#endif
#ifdef __cpp_lib_scoped_lock  // C++17
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
class shared_mutex;  // C++17
using shared_timed_mutex = shared_mutex;  // C++14
class recursive_mutex;
using recursive_timed_mutex = recursive_mutex;
enum class cv_status { no_timeout, timeout };
class condition_variable;
class thread;

template<typename _Tp>
class thread_specific_ptr;  // inspired by boost

// Provide limited support for `-fno-exceptions`.
#if defined __cpp_exceptions || defined __EXCEPTIONS || defined _CPPUNWIND
#  define __MCF_THROW_SYSTEM_ERROR(_Code, _Msg)  \
     throw ::std::system_error(static_cast<int>(::std::errc::_Code),  \
                               ::std::generic_category(), _Msg)
#else
#  define __MCF_THROW_SYSTEM_ERROR(_Code, _Msg)   ::__MCF_runtime_failure(_Msg)
#endif

// Define shortcuts for `enable_if`.
#define __MCF_SFINAE_ENABLE_IF(...)   typename ::std::enable_if<(bool) (__VA_ARGS__)>::type* = nullptr
#define __MCF_SFINAE_DISABLE_IF(...)  typename ::std::enable_if<!(bool) (__VA_ARGS__)>::type* = nullptr

// Declare prototypes for callback functions.
#if defined __cpp_noexcept_function_type
#  define __MCF_NOEXCEPT_ON_TYPEDEF   noexcept
#else
#  define __MCF_NOEXCEPT_ON_TYPEDEF
#endif

template<typename... _Args>
using _Vfn = void (_Args...);

template<typename... _Args>
using _Vfn_noexcept = void (_Args...) __MCF_NOEXCEPT_ON_TYPEDEF;

// This is the maximum integer representable as a `double` exact.
constexpr int64_t _Max_ms = 0x7FFFFFFFFFFFFC00;

// Convert a `chrono::duration` to the number of milliseconds, represented
// as a non-negative 64-bit integer. For everything other than milliseconds,
// it is necessary to perform calculation using a floating-point type and
// check for overflows before casting.
__MCF_CXX14(constexpr) inline
int64_t
__clamp_duration(const chrono::milliseconds& __ms)
  noexcept
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
__sleep_noninterruptible(int64_t* __timeout_opt)
  noexcept
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

// Provide `INVOKE` for C++11. [func.require]
// At the moment, all results are discarded, so the expression always has a
// type of `void` for simplicity.
#if defined __cpp_lib_invoke

template<class... _Args>
constexpr
void
__v_invoke(_Args&&... __args)
  { ::std::invoke(::std::forward<_Args>(__args)...);  }

#else  // __cpp_lib_invoke

template<class _Member, class _Class, class... _Args>
__MCF_CXX14(constexpr)
void
__v_invoke(_Member _Class::* __memp, _Args&&... __args)
  { ::std::mem_fn(__memp) (::std::forward<_Args>(__args)...);  }

template<class _Callable, class... _Args>
__MCF_CXX14(constexpr)
void
__v_invoke(_Callable&& __callable, _Args&&... __args)
  { ::std::forward<_Callable>(__callable) (::std::forward<_Args>(__args)...);  }

#endif  // __cpp_lib_invoke

// Create a thread. ISO C++ requires that arguments for the constructor of
// `std::thread` be passed as decay-copied rvalues. This is the object that
// saves copies of them and invokes the target function accordingly.
template<typename _Callable, typename... _Args>
struct _Invoke_decay_copy;

template<typename _Callable>
struct _Invoke_decay_copy<_Callable>
  {
    typename ::std::decay<_Callable>::type _M_callable;

    explicit _Invoke_decay_copy(_Callable& __callable)
      : _M_callable(::std::forward<_Callable>(__callable))
      {
      }

    template<typename... _Args>
    void
    __do_it(_Args&... __args)
      {
        _Noadl::__v_invoke(::std::move(this->_M_callable), ::std::move(__args)...);
      }
  };

template<typename _Callable, typename _Mine, typename... _Others>
struct _Invoke_decay_copy<_Callable, _Mine, _Others...>
  {
    typename ::std::decay<_Mine>::type _M_mine;
    _Invoke_decay_copy<_Callable, _Others...> _M_next;

    explicit _Invoke_decay_copy(_Callable& __callable, _Mine& __mine, _Others&... __others)
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
    __MCF_BR(::_MCF_once) _M_once = { };

    constexpr once_flag() noexcept = default;
    once_flag(const once_flag&) = delete;
    once_flag& operator=(const once_flag&) = delete;
  };

// Reference implementation for [thread.once.callonce]
template<typename _Callable, typename... _Args>
void
call_once(once_flag& __flag, _Callable&& __callable, _Args&&... __args)
  {
    struct _Once_sentry
      {
        _Vfn_noexcept<::_MCF_once*>* __deferred_fn;
        ::_MCF_once* __once;
        ~_Once_sentry() noexcept { (* this->__deferred_fn) (this->__once);  }
      };

    int __err = ::_MCF_once_wait(__flag._M_once, nullptr);
    if(__err == 0)
      return;  // passive

    // active
    __MCF_ASSERT(__err == 1);
    _Once_sentry __sentry = { ::_MCF_once_abort, __flag._M_once };
    _Noadl::__v_invoke(::std::forward<_Callable>(__callable), ::std::forward<_Args>(__args)...);
    __sentry.__deferred_fn = ::_MCF_once_release;
  }

// Reference implementation for [thread.mutex.class] and
// [thread.timedmutex.class].
class mutex
  {
  private:
    __MCF_BR(::_MCF_mutex) _M_mtx = { };

  public:
    constexpr mutex() noexcept = default;
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    using native_handle_type = ::_MCF_mutex*;
    using lock_guard_type = lock_guard<mutex>;  // extension
    using unique_lock_type = unique_lock<mutex>;  // extension

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle()
      noexcept
      {
        return this->_M_mtx;
      }

    void
    lock()
      noexcept  // strengthened
      {
        int __err = ::_MCF_mutex_lock(this->_M_mtx, nullptr);
        __MCF_ASSERT(__err == 0);
      }

    bool
    try_lock()
      noexcept  // strengthened
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
    unlock()
      noexcept  // strengthened
      {
        ::_MCF_mutex_unlock(this->_M_mtx);
      }
  };

// Reference implementation for [thread.sharedmutex.class] and
// [thread.sharedtimedmutex.class].
class shared_mutex
  {
  private:
    __MCF_BR(::_MCF_shared_mutex) _M_smtx = { };

  public:
    constexpr shared_mutex() noexcept = default;
    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    using native_handle_type = ::_MCF_shared_mutex*;
    using lock_guard_type = lock_guard<shared_mutex>;  // extension
    using unique_lock_type = unique_lock<shared_mutex>;  // extension
#if __MCF_CXX14(1+)0
    using shared_lock_type = shared_lock<shared_mutex>;  // extension
#endif

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle()
      noexcept
      {
        return this->_M_smtx;
      }

    void
    lock()
      noexcept  // strengthened
      {
        int __err = ::_MCF_shared_mutex_lock_exclusive(this->_M_smtx, nullptr);
        __MCF_ASSERT(__err == 0);
      }

    bool
    try_lock()
      noexcept  // strengthened
      {
        int64_t __timeout = 0;
        int __err = ::_MCF_shared_mutex_lock_exclusive(this->_M_smtx, &__timeout);
        return __err == 0;
      }

    template<typename _Clock, typename _Dur>
    bool
    try_lock_until(const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        int __err = _Noadl::__wait_until(__abs_time, ::_MCF_shared_mutex_lock_exclusive, this->_M_smtx);
        return __err == 0;
      }

    template<typename _Rep, typename _Period>
    bool
    try_lock_for(const chrono::duration<_Rep, _Period>& __rel_time)
      {
        int __err = _Noadl::__wait_for(__rel_time, ::_MCF_shared_mutex_lock_exclusive, this->_M_smtx);
        return __err == 0;
      }

    void
    unlock()
      noexcept  // strengthened
      {
        ::_MCF_shared_mutex_unlock(this->_M_smtx);
      }

    void
    lock_shared()
      noexcept  // strengthened
      {
        int __err = ::_MCF_shared_mutex_lock_shared(this->_M_smtx, nullptr);
        __MCF_ASSERT(__err == 0);
      }

    bool
    try_lock_shared()
      noexcept  // strengthened
      {
        int64_t __timeout = 0;
        int __err = ::_MCF_shared_mutex_lock_shared(this->_M_smtx, &__timeout);
        return __err == 0;
      }

    template<typename _Clock, typename _Dur>
    bool
    try_lock_shared_until(const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        int __err = _Noadl::__wait_until(__abs_time, ::_MCF_shared_mutex_lock_shared, this->_M_smtx);
        return __err == 0;
      }

    template<typename _Rep, typename _Period>
    bool
    try_lock_shared_for(const chrono::duration<_Rep, _Period>& __rel_time)
      {
        int __err = _Noadl::__wait_for(__rel_time, ::_MCF_shared_mutex_lock_shared, this->_M_smtx);
        return __err == 0;
      }

    void
    unlock_shared()
      noexcept  // strengthened
      {
        ::_MCF_shared_mutex_unlock(this->_M_smtx);
      }
  };

// Reference implementation for [thread.timedmutex.recursive] and
// [thread.timedmutex.recursive].
class recursive_mutex
  {
  private:
    __MCF_BR(::__MCF_gthr_rc_mutex) _M_rmtx = { };

  public:
    constexpr recursive_mutex() noexcept = default;  // strengthened
    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;

    using native_handle_type = ::__MCF_gthr_rc_mutex*;
    using lock_guard_type = lock_guard<recursive_mutex>;  // extension
    using unique_lock_type = unique_lock<recursive_mutex>;  // extension

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle()
      noexcept
      {
        return this->_M_rmtx;
      }

    void
    lock()
      noexcept  // strengthened
      {
        int __err = ::__MCF_gthr_rc_mutex_recurse(this->_M_rmtx);
        if(__err != 0)
          __err = ::__MCF_gthr_rc_mutex_wait(this->_M_rmtx, nullptr);
        __MCF_ASSERT(__err == 0);
      }

    bool
    try_lock()
      noexcept  // strengthened
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
    unlock()
      noexcept  // strengthened
      {
        ::__MCF_gthr_rc_mutex_release(this->_M_rmtx);
      }
  };

// Reference implementation for [thread.condition.condvar]
class condition_variable
  {
  private:
    __MCF_BR(::_MCF_cond) _M_cnd = { };

  public:
    constexpr condition_variable() noexcept = default;  // strengthened
    condition_variable(const condition_variable&) = delete;
    condition_variable& operator=(const condition_variable&) = delete;

    using native_handle_type = ::_MCF_cond*;
    using mutex_type = lock_guard<mutex>;  // extension
    using unique_lock_type = unique_lock<mutex>;  // extension

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle()
      noexcept
      {
        return this->_M_cnd;
      }

    void
    notify_one()
      noexcept
      {
        ::_MCF_cond_signal(this->_M_cnd);
      }

    void
    notify_all()
      noexcept
      {
        ::_MCF_cond_signal_all(this->_M_cnd);
      }

    void
    wait(unique_lock_type& __lock)
      noexcept  // strengthened
      {
        __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock_type __temp_lock;
        __temp_lock.swap(__lock);
        ::_MCF_mutex* __mtx = __temp_lock.mutex()->native_handle();
        int __err = ::__MCF_gthr_cond_mutex_wait(this->_M_cnd, __mtx, nullptr);
        __temp_lock.swap(__lock);
        __MCF_ASSERT(__err == 0);
      }

    template<typename _Predicate>
    void
    wait(unique_lock_type& __lock, _Predicate&& __pred)
      {
        while(!(bool) __pred())
          this->wait(__lock);
      }

    template<typename _Clock, typename _Dur>
    cv_status
    wait_until(unique_lock_type& __lock, const chrono::time_point<_Clock, _Dur>& __abs_time)
      {
        __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock_type __temp_lock;
        __temp_lock.swap(__lock);
        ::_MCF_mutex* __mtx = __temp_lock.mutex()->native_handle();
        int __err = _Noadl::__wait_until(__abs_time, ::__MCF_gthr_cond_mutex_wait, this->_M_cnd, __mtx);
        __temp_lock.swap(__lock);
        return (__err == 0) ? cv_status::no_timeout : cv_status::timeout;
      }

    template<typename _Clock, typename _Dur, typename _Predicate>
    bool
    wait_until(unique_lock_type& __lock, const chrono::time_point<_Clock, _Dur>& __abs_time, _Predicate&& __pred)
      {
        while(!(bool) __pred())
          if(this->wait_until(__lock, __abs_time) == cv_status::timeout)
            return __pred();
        return true;
      }

    template<typename _Rep, typename _Period>
    cv_status
    wait_for(unique_lock_type& __lock, const chrono::duration<_Rep, _Period>& __rel_time)
      {
        __MCF_ASSERT(__lock.owns_lock());  // must owning a mutex
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock_type __temp_lock;
        __temp_lock.swap(__lock);
        ::_MCF_mutex* __mtx = __temp_lock.mutex()->native_handle();
        int __err = _Noadl::__wait_for(__rel_time, ::__MCF_gthr_cond_mutex_wait, this->_M_cnd, __mtx);
        __temp_lock.swap(__lock);
        return (__err == 0) ? cv_status::no_timeout : cv_status::timeout;
      }

    template<typename _Rep, typename _Period, typename _Predicate>
    bool
    wait_for(unique_lock_type& __lock, const chrono::duration<_Rep, _Period>& __rel_time, _Predicate&& __pred)
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

    int __err = ::__MCF_cxa_thread_atexit(__MCF_CAST_PTR(__MCF_cxa_dtor_cdecl, ::_MCF_cond_signal_all),
                                          __cond.native_handle(), &__dso_handle);
    if(__err != 0)
      __MCF_THROW_SYSTEM_ERROR(not_enough_memory, "__MCF_cxa_thread_atexit");

    __err = ::__MCF_cxa_thread_atexit(__MCF_CAST_PTR(__MCF_cxa_dtor_cdecl, ::_MCF_mutex_unlock),
                                      __lock.mutex()->native_handle(), &__dso_handle);
    if(__err != 0)
      __MCF_THROW_SYSTEM_ERROR(not_enough_memory, "__MCF_cxa_thread_atexit");

    __lock.release();
  }

// Reference implementation for [thread.thread.class]
struct _Thread_id
  {
    uint32_t _M_tid;  // Windows thread ID

    constexpr _Thread_id() noexcept : _M_tid()  { }
    explicit constexpr _Thread_id(::_MCF_thread* __thr_opt) noexcept
      : _M_tid(__thr_opt ? ::_MCF_thread_get_tid(__thr_opt) : 0U)  { }
  };

class thread
  {
  private:
    ::_MCF_thread* _M_thr = nullptr;

  public:
    constexpr thread() noexcept = default;
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    template<typename _Callable, typename... _Args,
    __MCF_SFINAE_DISABLE_IF(::std::is_same<typename ::std::decay<_Callable>::type, thread>::value)>
    explicit thread(_Callable&& __callable, _Args&&... __args)
      {
        using _My_invoker = _Invoke_decay_copy<_Callable, _Args...>;
        enum { _St_zero, _St_constructed, _St_cancelled };

        struct _My_data
          {
            __MCF_BR(_My_invoker) _M_invoker;
            __MCF_BR(::_MCF_event) _M_ctor_status;
          };

        struct _Thread_sentry
          {
            _Vfn_noexcept<::_MCF_thread*>* __deferred_fn;
            ::_MCF_thread* __thr;
            ~_Thread_sentry() noexcept { (* this->__deferred_fn) (this->__thr);  }
          };

        auto __thread_fn = [](::_MCF_thread* __thr)
          {
            _My_data* const __my = (_My_data*) ::_MCF_thread_get_data(__thr);

            // Check whether `*_M_invoker` has been constructed. If its
            // constructor failed, this thread shall exit immediately.
            int __st = ::_MCF_event_await_change(__my->_M_ctor_status, _St_zero, nullptr);
            if(__st == _St_cancelled)
              return;

            // Execute the user-defined procedure.
            __MCF_ASSERT(__st == _St_constructed);
            __my->_M_invoker->__do_it();
            __my->_M_invoker->~_My_invoker();
          };

        auto __sentry_cancel_thread = [](::_MCF_thread* __thr) noexcept
          {
            _My_data* const __my = (_My_data*) ::_MCF_thread_get_data(__thr);

            // Cancel the thread.
            ::_MCF_event_set(__my->_M_ctor_status, _St_cancelled);
            ::_MCF_thread_drop_ref(__thr);
          };

        auto __sentry_complete_thread = [](::_MCF_thread* __thr) noexcept
          {
            _My_data* const __my = (_My_data*) ::_MCF_thread_get_data(__thr);

            // Let the thread go.
            ::_MCF_event_set(__my->_M_ctor_status, _St_constructed);
          };

        // Create the thread. User-defined data are initialized to zeroes.
        this->_M_thr = ::_MCF_thread_new_aligned(__thread_fn, alignof(_My_data), nullptr, sizeof(_My_data));
        if(!this->_M_thr)
          __MCF_THROW_SYSTEM_ERROR(resource_unavailable_try_again, "_MCF_thread_new_aligned");

        // active
        _Thread_sentry __sentry = { __sentry_cancel_thread, this->_M_thr };
        ::new(::_MCF_thread_get_data(this->_M_thr)) _My_invoker(__callable, __args...);
        __sentry.__deferred_fn = __sentry_complete_thread;
      }

    thread(thread&& __other)
      noexcept
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
    using id = _Thread_id;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle()
      noexcept
      {
        return this->_M_thr;
      }

    void
    swap(thread& __other)
      noexcept
      {
        ::std::swap(this->_M_thr, __other._M_thr);
      }

    bool
    joinable()
      const noexcept
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
    detach()
      noexcept  // strengthened
      {
        __MCF_ASSERT(this->_M_thr);  // must be joinable

        ::_MCF_thread_drop_ref(this->_M_thr);
        this->_M_thr = nullptr;
      }

    __MCF_CXX14(constexpr)
    id
    get_id()
      const noexcept
      {
        return id(this->_M_thr);
      }

    static
    unsigned
    hardware_concurrency()
      noexcept
      {
        return (unsigned) ::_MCF_get_processor_count();
      }
  };

inline
void
swap(thread& __lhs, thread& __rhs)
  noexcept
  {
    __lhs.swap(__rhs);
  }

constexpr
bool
operator==(thread::id __x, thread::id __y)
  noexcept
  {
    return __x._M_tid == __y._M_tid;
  }

#ifdef __cpp_lib_three_way_comparison

constexpr
::std::strong_ordering
operator<=>(thread::id __x, thread::id __y)
  noexcept
  {
    return __x._M_tid <=> __y._M_tid;
  }

#else  // __cpp_lib_three_way_comparison

constexpr
bool
operator!=(thread::id __x, thread::id __y)
  noexcept
  {
    return __x._M_tid != __y._M_tid;
  }

constexpr
bool
operator<(thread::id __x, thread::id __y)
  noexcept
  {
    return __x._M_tid < __y._M_tid;
  }

constexpr
bool
operator>(thread::id __x, thread::id __y)
  noexcept
  {
    return __x._M_tid > __y._M_tid;
  }

constexpr
bool
operator<=(thread::id __x, thread::id __y)
  noexcept
  {
    return __x._M_tid <= __y._M_tid;
  }

constexpr
bool
operator>=(thread::id __x, thread::id __y)
  noexcept
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
    get_id()
      noexcept
      {
        thread::id __id;
        __id._M_tid = ::_MCF_thread_self_tid();
        return __id;
      }

    inline
    void
    yield()
      noexcept  // strengthened
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

  public:
    thread_specific_ptr(const thread_specific_ptr&) = delete;
    thread_specific_ptr& operator=(const thread_specific_ptr&) = delete;

    thread_specific_ptr()
      {
        this->_M_key = ::_MCF_tls_key_new(+[](void* __vptr) { delete (_Tp*) __vptr; });
        if(!this->_M_key)
          __MCF_THROW_SYSTEM_ERROR(resource_unavailable_try_again, "_MCF_tls_key_new");
      }

    explicit thread_specific_ptr(_Vfn<_Tp*>* __cleanup_opt)
      {
        this->_M_key = ::_MCF_tls_key_new(__MCF_CAST_PTR(::_MCF_tls_dtor, __cleanup_opt));
        if(!this->_M_key)
          __MCF_THROW_SYSTEM_ERROR(resource_unavailable_try_again, "_MCF_tls_key_new");
      }

    ~thread_specific_ptr()
      {
        ::_MCF_tls_key_delete(this->_M_key);
      }

    using native_handle_type = ::_MCF_tls_key*;
    using element_type = _Tp;
    using pointer = _Tp*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle()
      noexcept
      {
        return this->_M_key;
      }

    pointer
    get()
      const noexcept
      {
        return (pointer) ::_MCF_tls_get(this->_M_key);
      }

    explicit operator bool()
      const noexcept
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
    release()
      noexcept
      {
        void* __ptr_old = nullptr;
        ::_MCF_tls_xset(this->_M_key, &__ptr_old, nullptr);
        return (pointer) __ptr_old;
      }
  };

}  /* namespace MCF  */
#endif  /* __MCFGTHREAD_CXX11_  */
