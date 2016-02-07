// Copyright (c) 2106 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_IS_RUNNING_HPP
#define BOOST_PROCESS_POSIX_IS_RUNNING_HPP

#include <boost/process/config.hpp>
#include <boost/system/error_code.hpp>
#include <signal.h>

namespace boost { namespace process { namespace posix {

template <class Process>
bool is_running(const Process &p)
{
    if (::kill(p.pid, 0) == -1)
    {
        auto last_error = errno;


        if (last_error == ESRCH)
            return false;

        boost::system::error_code ec(errno, boost::system::system_category());
        boost::throw_exception(
                               boost::system::system_error(
                                   ec, "is_running error"
                               )
                           );
        //BOOST_PROCESS_THROW_LAST_SYSTEM_ERROR("kill(2) failed");
    }
    else
        return true;

}

template <class Process>
bool is_running(const Process &p, boost::system::error_code &ec)
{
    if (::kill(p.pid, 0) == -1)
    {
        auto last_error = errno;
        if (last_error == ESRCH)
            return false;

        ec.assign(last_error, boost::system::system_category());

        BOOST_PROCESS_RETURN_LAST_SYSTEM_ERROR(ec);
    }
    else
    {
        ec.clear();
        return true;
    }
}

}}}

#endif
