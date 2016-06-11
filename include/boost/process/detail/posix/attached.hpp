// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_POSIX_ATTACHED_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_ATTACHED_HPP_


#include <boost/process/detail/posix/handler.hpp>
#include <signal.h>
#include <sys/prctl.h>


namespace boost { namespace process { namespace detail { namespace posix {


struct attached : handler_base_ext
{

    template <class Executor>
    void on_setup(Executor &e) const
    {
        signal(SIGCHLD, SIG_IGN);
    }

};


}}}}



#endif /* BOOST_PROCESS_DETAIL_HANDLER_HPP_ */
