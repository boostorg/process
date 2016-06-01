// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_SIGNAL_HPP
#define BOOST_PROCESS_POSIX_SIGNAL_HPP

#include <boost/process/detail/posix/handler.hpp>
#include <signal.h>

namespace boost { namespace process { namespace detail { namespace posix {

struct sig_ign_ : handler_base_ext
{
    constexpr sig_ign_ () {}

    template <class PosixExecutor>
    void on_exec_setup(PosixExecutor&) const
    {
    	::signal(SIGCHLD, SIG_IGN);
    }
};


constexpr static sig_ign_ sig_ign;

struct sig_dfl_ : handler_base_ext
{
    constexpr sig_dfl_ () {}

    template <class PosixExecutor>
    void on_exec_setup(PosixExecutor&) const
    {
    	::signal(SIGCHLD, SIG_DFL);
    }
};


constexpr static sig_dfl_ sig_dfl;

struct sig_init_ : handler_base_ext
{
    sig_init_ (::sighandler_t handler) : _handler(handler) {}

    template <class PosixExecutor>
    void on_exec_setup(PosixExecutor&) const
    {
    	::signal(SIGCHLD, _handler);
    }
private:
    ::sighandler_t _handler;
};

struct sig_
{
	constexpr sig_() {}

	sig_init_ operator()(::sighandler_t h) const {return h;}
	sig_init_ operator= (::sighandler_t h) const {return h;}
};


constexpr static sig_ sig;



}}}}

#endif
