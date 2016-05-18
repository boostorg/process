// Copyright (c) 2106 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_IS_RUNNING_HPP
#define BOOST_PROCESS_DETAIL_POSIX_IS_RUNNING_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/posix/child_handle.hpp>
#include <system_error>
#include <signal.h>

namespace boost { namespace process { namespace detail {namespace posix {


inline bool is_running(const child_handle &p)
{
    if (::kill(p.pid, 0) == -1)
    {
        auto last_error = errno;


        if (last_error == ESRCH)
            return false;

        ::boost::process::detail::throw_last_error("is_running error");
        return false;
    }
    else
        return true;

}

inline bool is_running(const child_handle &p, std::error_code &ec)
{
    if (::kill(p.pid, 0) == -1)
    {
        auto last_error = errno;
        if (last_error == ESRCH)
            return false;

        ec = ::boost::process::detail::get_last_error();
    }
    else
    {
        ec.clear();
        return true;
    }
}

}}}}

#endif
