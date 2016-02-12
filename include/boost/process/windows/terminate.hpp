// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_TERMINATE_HPP
#define BOOST_PROCESS_WINDOWS_TERMINATE_HPP

#include <boost/process/config.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/get_last_error.hpp>

namespace boost { namespace process { namespace windows {

template <class Process>
void terminate(const Process &p)
{
    if (!::boost::detail::winapi::TerminateProcess(p.process_handle(), EXIT_FAILURE))
        throw std::system_error(
                std::error_code(
                ::boost::detail::winapi::GetLastError(),
                boost::system::system_category()),
                "TerminateProcess() failed");//position of this is not really interesting
}

template <class Process>
void terminate(const Process &p, std::error_code &ec)
{
    if (!::boost::detail::winapi::TerminateProcess(p.process_handle(), EXIT_FAILURE))
        ec = std::error_code(
                ::boost::detail::winapi::GetLastError(),
                std::system_category());
    else
        ec.clear();
}

}}}

#endif
