// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_EXE_HPP
#define BOOST_PROCESS_V2_EXE_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>

#include <boost/process/v2/process_handle.hpp>
#include <boost/process/v2/pid.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

/// Return the executable path from pid
BOOST_PROCESS_V2_DECL filesystem::path exe(pid_type pid, error_code & ec);
BOOST_PROCESS_V2_DECL filesystem::path exe(pid_type pid);

#if defined(BOOST_PROCESS_V2_WINDOWS)
BOOST_PROCESS_V2_DECL filesystem::path exe(HANDLE handle, error_code & ec);
BOOST_PROCESS_V2_DECL filesystem::path exe(HANDLE handle);
#endif

template<typename Executor>
BOOST_PROCESS_V2_DECL filesystem::path exe(basic_process_handle<Executor> & handle, error_code & ec)
{
    return exe(handle.native_handle(), ec);
}

template<typename Executor>
BOOST_PROCESS_V2_DECL filesystem::path exe(basic_process_handle<Executor> & handle)
{
    return exe(handle.native_handle());
}

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_EXE_HPP

