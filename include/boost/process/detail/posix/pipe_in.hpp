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
#include <boost/process/detail/posix/handler.hpp>
#include <unistd.h>


namespace boost { namespace process { namespace detail { namespace posix {

struct pipe_in : handler_base_ext
{
    int descr_;

    pipe_in(int descr) : descr_(descr) {}

    template<typename T>
    pipe_in(T & p) : descr_(p.native_source())
	{
    	p.assign_source(-1);
	}

    template<typename Executor>
    void on_error(Executor &, const std::error_code &) const
    {
        ::close(descr_);
    }

    template<typename Executor>
    void on_success(Executor &) const
    {
        ::close(descr_);
    }

    template <class Executor>
    void on_exec_setup(Executor &e) const
    {
        if (::dup2(descr_, STDIN_FILENO) == -1)
             e.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
        ::close(descr_);
    }

};

class async_pipe;

template<typename = void>
struct async_pipe_in : public pipe_in
{
	boost::asio::posix::stream_descriptor handle;

    async_pipe_in(async_pipe & p) : pipe_in(p.native_source()),
	                       handle(std::move(p).source())
	{
	}

    template<typename Executor>
    void on_error(Executor &, const std::error_code &)
    {
    	boost::system::error_code ec;
    	handle.close(ec);
    }

    template<typename Executor>
    void on_success(Executor &)
    {
    	boost::system::error_code ec;
    	handle.close(ec);
    }
};

}}}}

#endif
