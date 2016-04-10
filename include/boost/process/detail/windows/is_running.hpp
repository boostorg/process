// Copyright (c) 2106 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_IS_RUNNING_HPP
#define BOOST_PROCESS_WINDOWS_IS_RUNNING_HPP

#include <boost/process/detail/config.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <boost/detail/winapi/process.hpp>

namespace boost { namespace process { namespace detail { namespace windows {

template <class Process>
bool is_running(const Process &p)
{
    ::boost::detail::winapi::DWORD_ code;
    //single value, not needed in the winapi.
    static constexpr ::boost::detail::winapi::DWORD_ still_active = 259;

    if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &code))
        ::boost::process::detail::throw_last_error("GetExitCodeProcess() failed");


    return code == still_active;
}

template <class Process>
bool is_running(const Process &p, std::error_code &ec)
{
    ::boost::detail::winapi::DWORD_ code;
    //single value, not needed in the winapi.
    static constexpr ::boost::detail::winapi::DWORD_ still_active = 259;

    if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &code))
        ec = ::boost::process::detail::get_last_error();
    else
        ec.clear();

    return code == still_active;
}

}}}}

#endif
