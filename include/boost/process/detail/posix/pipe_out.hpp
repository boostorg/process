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
#include <boost/process/detail/posix/handler.hpp>
#include <unistd.h>

namespace boost { namespace process { namespace detail { namespace posix {

template<int p1, int p2>
struct pipe_out : handler_base_ext
{
    int descr_;

    pipe_out(int descr) : descr_(descr) {}

    template<typename T>
    pipe_out(T & p) : descr_(p.native_sink())
	{
    	p.assign_sink(-1);
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

    template <typename Executor>
    void on_exec_setup(Executor &e) const;
};

template<>
template<typename Executor>
void pipe_out<1,-1>::on_exec_setup(Executor &e) const
{
    if (::dup2(descr_, STDOUT_FILENO) == -1)
         e.set_error(::boost::process::detail::get_last_error(), "dup3() failed");
    ::close(descr_);

}

template<>
template<typename Executor>
void pipe_out<2,-1>::on_exec_setup(Executor &e) const
{
    if (::dup2(descr_, STDERR_FILENO) == -1)
         e.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
    ::close(descr_);
}

template<>
template<typename Executor>
void pipe_out<1,2>::on_exec_setup(Executor &e) const
{
    if (::dup2(descr_, STDOUT_FILENO) == -1)
         e.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
    if (::dup2(descr_, STDERR_FILENO) == -1)
         e.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
    ::close(descr_);
}

class async_pipe;

template<int p1, int p2>
struct async_pipe_out : public pipe_out<p1, p2>
{
	async_pipe &pipe;
	template<typename AsyncPipe>
    async_pipe_out(AsyncPipe & p) : pipe_out<p1, p2>(p.native_sink()), pipe(p)
    {
    }

	template<typename Pipe, typename Executor>
	static void close(Pipe & pipe, Executor &)
	{
    	boost::system::error_code ec;
    	std::move(pipe).sink().close(ec);
	}

    template<typename Executor>
    void on_error(Executor & exec, const std::error_code &)
    {
    	close(pipe, exec);
    }

    template<typename Executor>
    void on_success(Executor &exec)
    {
    	close(pipe, exec);
    }
};


}}}}

#endif
