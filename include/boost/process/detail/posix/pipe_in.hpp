// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_PIPE_IN_HPP
#define BOOST_PROCESS_POSIX_PIPE_IN_HPP

#include <boost/process/pipe.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/process/detail/posix/handler.hpp>
#include <unistd.h>


namespace boost { namespace process { namespace detail { namespace posix {

struct pipe_in : handler_base_ext
{
    int descr_;
    template<typename CharT, typename Traits>
    pipe_in(const boost::process::basic_pipe<CharT, Traits> & p) : descr_(p.native_source()) {}
    pipe_in(const boost::process::async_pipe & p)                : descr_(p.native_source()) {}


    template <class Executor>
    void on_exec_setup(Executor &e) const
    {
        ::dup2(descr_, STDIN_FILENO);
    }
};

}}}}

#endif
