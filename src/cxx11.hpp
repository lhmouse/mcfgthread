/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_CXX11_
#define __MCFGTHREAD_CXX11_

#include "fwd.h"
#include "gthr_aux.h"
#include "cxa.h"
#include <mutex>  // lock_guard, unique_lock
#include <chrono>  // duration, time_point
#include <functional>  // ref(), invoke()
#include <system_error>  // system_error, errc, error_code
#include <cmath>  // isfinite
#include <type_traits>

namespace MCF {
namespace _Noadl = ::MCF;

// Forward declarations
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
using ::std::swap;

struct once_flag;
class mutex;
using timed_mutex = mutex;
class recursive_mutex;
using recursive_timed_mutex = recursive_mutex;
class condition_variable;
enum class cv_status { no_timeout, timeout };
class thread;
class atomic_flag;

template<typename _Tp>
class thread_specific_ptr;  // inspired by boost

template<typename _Tp>
class thread_ptr;

// Auxiliary functions
#if defined(__EXCEPTIONS) || defined(__cpp_exceptions)

#  define __MCF_CXX11_TRY          try
#  define __MCF_CXX11_CATCH(...)   catch(__VA_ARGS__)
#  define __MCF_CXX11_THROW(...)   throw __VA_ARGS__

#  define __MCF_CXX11_THROW_SYSTEM_ERROR(C, S)  \
     throw ::std::system_error(  \
         (int) (::std::errc::C), ::std::generic_category(), (S))

#else  // __EXCEPTIONS

#  define __MCF_CXX11_TRY          if(true)
#  define __MCF_CXX11_CATCH(...)   else
#  define __MCF_CXX11_THROW(...)   ::std::terminate()

#  define __MCF_CXX11_THROW_SYSTEM_ERROR(C, S)  \
     ::__MCF_runtime_failure(S)

#endif  // __EXCEPTIONS

template<typename _Duration>
inline
chrono::duration<double, ::std::milli>
__hires_duration_cast(const _Duration& __dur)
  {
    auto __ms_dur = chrono::duration_cast<chrono::duration<double, ::std::milli>>(__dur);
    if(!::std::isfinite(__ms_dur.count()))
      __MCF_CXX11_THROW_SYSTEM_ERROR(argument_out_of_domain, "chrono::duration_cast");
    return __ms_dur;
  }

template<typename _Callable, typename... _Args>
inline
int
__wait_until(const chrono::time_point<chrono::system_clock, chrono::milliseconds>& __abs_time,
             _Callable __func, _Args... __args)
  {
    int64_t __timeout = 0;  // immediate
    auto __ms_dur = __abs_time.time_since_epoch();

    if(__ms_dur.count() >= 0)
      __timeout = + __ms_dur.count();  // absolute

    return __func(__args..., &__timeout);
  }

template<typename _Duration, typename _Callable, typename... _Args>
inline
int
__wait_until(const chrono::time_point<chrono::system_clock, _Duration>& __abs_time,
             _Callable __func, _Args... __args)
  {
    int64_t __timeout = 0;  // immediate
    auto __ms_dur = _Noadl::__hires_duration_cast(__abs_time.time_since_epoch());

    if(__ms_dur.count() >= 0x7FFFFFFFFFFFFC00)
      __timeout = INT64_MAX;  // infinite
    else if(__ms_dur.count() >= 0)
      __timeout = + (int64_t) (__ms_dur.count() + 0.9999999);  // absolute

    return __func(__args..., &__timeout);
  }

template<typename _Clock, typename _Duration, typename _Callable, typename... _Args>
inline
int
__wait_until(const chrono::time_point<_Clock, _Duration>& __abs_time,
             _Callable __func, _Args... __args)
  {
    int64_t __timeout;
    int __err;

    do {
      __timeout = 0;  // immediate
      auto __ms_dur = _Noadl::__hires_duration_cast(__abs_time - _Clock::now());

      if(__ms_dur.count() >= 0x7FFFFFFFFFFFFC00)
        __timeout = INT64_MAX;  // infinite
      else if(__ms_dur.count() >= 0)
        __timeout = - (int64_t) (__ms_dur.count() + 0.9999999);  // relative

      __err = __func(__args..., &__timeout);
    }
    while((__timeout != 0) && (__err != 0));  // loop if timed out

    return __err;
  }

template<typename _Callable, typename... _Args>
inline
int
__wait_for(const chrono::milliseconds& __rel_time, _Callable __func, _Args... __args)
  {
    int64_t __timeout = 0;  // immediate

    if(__rel_time.count() >= 0)
      __timeout = - __rel_time.count();  // relative

    return __func(__args..., &__timeout);
  }

template<typename _Duration, typename _Callable, typename... _Args>
inline
int
__wait_for(const _Duration& __rel_time, _Callable __func, _Args... __args)
  {
    int64_t __timeout = 0;  // immediate
    auto __ms_dur = _Noadl::__hires_duration_cast(__rel_time);

    if(__ms_dur.count() >= 0x7FFFFFFFFFFFFC00)
      __timeout = INT64_MAX;  // infinite
    else if(__ms_dur.count() >= 0)
      __timeout = - (int64_t) (__ms_dur.count() + 0.9999999);  // relative

    return __func(__args..., &__timeout);
  }

extern "C" void* __dso_handle;

// Reference implementation for [thread.once.onceflag]
struct once_flag
  {
    ::_MCF_once _M_once[1] = { };

    constexpr once_flag() noexcept = default;
    once_flag(const once_flag&) = delete;
    once_flag& operator=(const once_flag&) = delete;
  };

// Reference implementation for [thread.once.callonce]
template<typename _Callable, typename... _Args>
void
call_once(once_flag& __flag, _Callable&& __func, _Args&&... __args)
  {
    // passive check
    if(::_MCF_once_wait(__flag._M_once, nullptr) == 0)
      return;

    __MCF_CXX11_TRY {
#if defined(__cpp_lib_invoke)
      ::std::invoke(::std::forward<_Callable>(__func), ::std::forward<_Args>(__args)...);
#else
      ::std::ref(__func)(::std::forward<_Args>(__args)...);
#endif
    }
    __MCF_CXX11_CATCH(...) {
      ::_MCF_once_abort(__flag._M_once);
      __MCF_CXX11_THROW();
    }
    ::_MCF_once_release(__flag._M_once);
  }

// Reference implementation for [thread.mutex.class]
class mutex
  {
  private:
    ::_MCF_mutex _M_mtx[1] = { };

  public:
    constexpr mutex() noexcept = default;
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

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

    template<typename _Clock, typename _Duration>
    bool
    try_lock_until(const chrono::time_point<_Clock, _Duration>& __abs_time)
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

    using native_handle_type = ::_MCF_mutex*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept  // strengthened
      {
        return this->_M_mtx;
      }
  };

// Reference implementation for [thread.timedmutex.recursive]
class recursive_mutex
  {
  private:
    ::__MCF_gthr_rc_mutex _M_rmtx[1] = { };

  public:
    constexpr recursive_mutex() noexcept = default;  // strengthened
    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;

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

    template<typename _Clock, typename _Duration>
    bool
    try_lock_until(const chrono::time_point<_Clock, _Duration>& __abs_time)
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

    using native_handle_type = ::__MCF_gthr_rc_mutex*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept  // strengthened
      {
        return this->_M_rmtx;
      }
  };

// Reference implementation for [thread.condition.condvar]
class condition_variable
  {
  private:
    ::_MCF_cond _M_cnd[1] = { };

  public:
    constexpr condition_variable() noexcept = default;  // strengthened
    condition_variable(const condition_variable&) = delete;
    condition_variable& operator=(const condition_variable&) = delete;

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
        __MCF_ASSERT(__lock.owns_lock());
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
    wait(unique_lock<mutex>& __lock, _Predicate __pred)
      {
        while(! __pred())
          this->wait(__lock);
      }

    template<typename _Clock, typename _Duration>
    cv_status
    wait_until(unique_lock<mutex>& __lock, const chrono::time_point<_Clock, _Duration>& __abs_time)
      {
        __MCF_ASSERT(__lock.owns_lock());
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock<mutex> __temp_lock;
        __temp_lock.swap(__lock);

        int __err = _Noadl::__wait_until(__abs_time, ::_MCF_cond_wait, this->_M_cnd,
             ::__MCF_gthr_mutex_unlock_callback, ::__MCF_gthr_mutex_relock_callback,
             (intptr_t) __temp_lock.mutex()->native_handle(), nullptr);

        __temp_lock.swap(__lock);
        return (__err == 0) ? cv_status::no_timeout : cv_status::timeout;
      }

    template<typename _Clock, typename _Duration, typename _Predicate>
    bool
    wait_until(unique_lock<mutex>& __lock, const chrono::time_point<_Clock, _Duration>& __abs_time,
               _Predicate __pred)
      {
        while(! __pred())
          if(this->wait_until(__lock, __abs_time) == cv_status::timeout)
            return __pred();
        return true;
      }

    template<typename _Rep, typename _Period>
    cv_status
    wait_for(unique_lock<mutex>& __lock, const chrono::duration<_Rep, _Period>& __rel_time)
      {
        __MCF_ASSERT(__lock.owns_lock());
        __MCF_ASSERT(__lock.mutex() != nullptr);

        unique_lock<mutex> __temp_lock;
        __temp_lock.swap(__lock);

        int __err = _Noadl::__wait_for(__rel_time, ::_MCF_cond_wait, this->_M_cnd,
             ::__MCF_gthr_mutex_unlock_callback, ::__MCF_gthr_mutex_relock_callback,
             (intptr_t) __temp_lock.mutex()->native_handle(), nullptr);

        __temp_lock.swap(__lock);
        return (__err == 0) ? cv_status::no_timeout : cv_status::timeout;
      }

    template<typename _Rep, typename _Period, typename _Predicate>
    bool
    wait_for(unique_lock<mutex>& __lock, const chrono::duration<_Rep, _Period>& __rel_time,
             _Predicate __pred)
      {
        return this->wait_until(__lock, chrono::steady_clock::now() + __rel_time,
                                ::std::move(__pred));
      }

    using native_handle_type = ::_MCF_cond*;

    __MCF_CXX14(constexpr)
    native_handle_type
    native_handle() noexcept  // strengthened
      {
        return this->_M_cnd;
      }
  };

// Reference implementation for [thread.condition.nonmember]
inline
void
notify_all_at_thread_exit(condition_variable& __cond, unique_lock<mutex> __lock)
  {
    __MCF_ASSERT(__lock.owns_lock());
    __MCF_ASSERT(__lock.mutex() != nullptr);

    ::__MCF_cxa_thread_atexit(
        +[](void* __ptr) { ((condition_variable*) __ptr)->notify_all();  },
        &__cond, &__dso_handle);

    ::__MCF_cxa_thread_atexit(
        +[](void* __ptr) { ((mutex*) __ptr)->unlock();  },
        __lock.release(), &__dso_handle);
  }

// Reference implementation for [thread.thread.class]
class thread
  {
  private:
    ::_MCF_thread* _M_thrd = nullptr;

  public:
    constexpr thread() noexcept = default;
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    
  };

}  /* namespace MCF  */
#endif  /* __MCFGTHREAD_CXX11_  */
