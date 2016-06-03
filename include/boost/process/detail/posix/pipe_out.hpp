// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_PIPE_OUT_HPP
#define BOOST_PROCESS_DETAIL_POSIX_PIPE_OUT_HPP

#include <boost/process/pipe.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/process/detail/posix/handler.hpp>
#include <unistd.h>

namespace boost { namespace process { namespace detail { namespace posix {

template<int p1, int p2>
struct pipe_out : handler_base_ext
{
    int descr_;
    template<typename CharT, typename Traits>
    pipe_out(const boost::process::basic_pipe<CharT, Traits> & p) : descr_(p.native_sink()) {}
    pipe_out(const boost::process::async_pipe & p)                : descr_(p.native_sink()) {}


    template <typename Executor>
    void on_exec_setup(Executor &e) const;
};

template<>
template<typename Executor>
void pipe_out<1,-1>::on_exec_setup(Executor &e) const
{
    ::dup2(descr_, STDOUT_FILENO);
}

template<>
template<typename Executor>
void pipe_out<2,-1>::on_exec_setup(Executor &e) const
{
    ::dup2(descr_, STDERR_FILENO);
}

template<>
template<typename Executor>
void pipe_out<1,2>::on_exec_setup(Executor &e) const
{
    ::dup2(descr_, STDOUT_FILENO);
    ::dup2(descr_, STDERR_FILENO);
}

}}}}

#endif
