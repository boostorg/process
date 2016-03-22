// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_WAIT_FOR_EXIT_HPP
#define BOOST_PROCESS_WINDOWS_WAIT_FOR_EXIT_HPP

#include <boost/process/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/detail/winapi/synchronization.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/process/windows/child_handle.hpp>
#include <chrono>

namespace boost { namespace process { namespace detail { namespace windows {

inline void wait(const child_handle &p, int & exit_code)
{
    if (::boost::detail::winapi::WaitForSingleObject(p.process_handle(),
        ::boost::detail::winapi::infinite) == ::boost::detail::winapi::wait_failed)
            throw std::system_error(
                std::error_code(
                ::boost::detail::winapi::GetLastError(),
                std::system_category()),
                "WaitForSingleObject() failed");

    ::boost::detail::winapi::DWORD_ _exit_code;
    if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &_exit_code))
        throw std::system_error(
                std::error_code(
                ::boost::detail::winapi::GetLastError(),
                 std::system_category()),
                "GetExitCodeProcess() failed");

    exit_code = static_cast<int>(_exit_code);
}

inline void wait(const child_handle &p, int & exit_code, std::error_code &ec)
{
	::boost::detail::winapi::DWORD_ _exit_code = 1;

    if (::boost::detail::winapi::WaitForSingleObject(p.process_handle(),
        ::boost::detail::winapi::infinite) == ::boost::detail::winapi::wait_failed)
            ec = std::error_code(
                ::boost::detail::winapi::GetLastError(),
                std::system_category());
    else if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &_exit_code))
            ec = std::error_code(
                ::boost::detail::winapi::GetLastError(),
                std::system_category());
    else
        ec.clear();

    exit_code = static_cast<int>(_exit_code);
}


template< class Rep, class Period >
inline bool wait_for(
        const child_handle &p,
        int & exit_code,
        const std::chrono::duration<Rep, Period>& rel_time)
{

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(rel_time);

    ::boost::detail::winapi::DWORD_ wait_code;
    wait_code = ::boost::detail::winapi::WaitForSingleObject(p.process_handle(), ms.count());
    if (wait_code == ::boost::detail::winapi::wait_failed)
        throw std::system_error(
            std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category()),
            "WaitForSingleObject() failed");
    else if (wait_code == ::boost::detail::winapi::wait_timeout)
        return false; //

    ::boost::detail::winapi::DWORD_ _exit_code;
    if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &_exit_code))
        throw std::system_error(
                std::error_code(
                ::boost::detail::winapi::GetLastError(),
                std::system_category()),
                "GetExitCodeProcess() failed");

    exit_code = static_cast<int>(_exit_code);
}


template< class Rep, class Period >
inline bool wait_for(
        const child_handle &p,
        int & exit_code,
        const std::chrono::duration<Rep, Period>& rel_time,
        std::error_code &ec)
{

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(rel_time);


    ::boost::detail::winapi::DWORD_ wait_code;
    wait_code = ::boost::detail::winapi::WaitForSingleObject(p.process_handle(), ms.count());
    if (wait_code == ::boost::detail::winapi::wait_failed)
        ec = std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category());
    else if (wait_code == ::boost::detail::winapi::wait_timeout)
        return false; //

    ::boost::detail::winapi::DWORD_ _exit_code = 1;
    if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &_exit_code))
    {
        ec = std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category());
        return false;
    }
    else
        ec.clear();

    exit_code = static_cast<int>(_exit_code);
    return true;
;
}

template< class Clock, class Duration >
inline bool wait_until(
        const child_handle &p,
        int & exit_code,
       const std::chrono::time_point<Clock, Duration>& timeout_time)
{
    std::chrono::milliseconds ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    timeout_time - std::chrono::system_clock::now());

    ::boost::detail::winapi::DWORD_ wait_code;
    wait_code = ::boost::detail::winapi::WaitForSingleObject(p.process_handle(), ms.count());

    if (wait_code == ::boost::detail::winapi::wait_failed)
        throw std::system_error(
            std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category()),
            "WaitForSingleObject() failed");
    else if (wait_code == ::boost::detail::winapi::wait_timeout)
        return false;

    ::boost::detail::winapi::DWORD_ _exit_code;
    if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &_exit_code))
        throw std::system_error(
            std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category()),
            "GetExitCodeProcess() failed");

    exit_code = static_cast<int>(_exit_code);

    return true;
}


template< class Clock, class Duration >
inline bool wait_until(
        const child_handle &p,
        int & exit_code,
        const std::chrono::time_point<Clock, Duration>& timeout_time,
        std::error_code &ec)
{
    std::chrono::milliseconds ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    timeout_time - std::chrono::system_clock::now());

    ::boost::detail::winapi::DWORD_ _exit_code = 1;

    if (::boost::detail::winapi::WaitForSingleObject(p.process_handle(),
            ms.count()) == ::boost::detail::winapi::wait_failed)
        ec = std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category());
    else if (!::boost::detail::winapi::GetExitCodeProcess(p.process_handle(), &_exit_code))
        ec = std::error_code(
            ::boost::detail::winapi::GetLastError(),
            std::system_category());
    else
        ec.clear();

    return static_cast<int>(exit_code);
;
}


}}}}

#endif
