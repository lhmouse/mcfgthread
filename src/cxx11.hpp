/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_CXX11_
#define __MCFGTHREAD_CXX11_

#include "fwd.h"
#include "gthr_aux.h"
#include "fwd.h"
#include "gthr_aux.h"
#include <mutex>  // lock_guard, unique_lock
#include <chrono>  // duration, time_point
#include <functional>  // ref
#include <system_error>  // system_error, errc, error_code

namespace MCF {

// Forward declarations
using ::std::lock_guard;
using ::std::unique_lock;
#ifdef __cpp_lib_scoped_lock
using ::std::scoped_lock;
#endif
namespace chrono = ::std::chrono;

class once_flag;
class mutex;
class recursive_mutex;
class condition_variable;
class thread;
class atomic_flag;

template<typename _Tp>
class thread_specific_ptr;  // inspired by boost

template<typename _Tp>
class thread_ptr;

// Auxiliary functions

// Reference implementation for `std::once_flag`


}  /* namespace MCF  */
#endif  /* __MCFGTHREAD_CXX11_  */
