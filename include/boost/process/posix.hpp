// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_POSIX_HPP_
#define BOOST_PROCESS_POSIX_HPP_

#include <boost/process/detail/posix/fd.hpp>
#include <boost/process/detail/posix/handler.hpp>


/** \file boost/process/posix.hpp
 *
 *	Header which provides the posix extensions. Those include fd, on_fork_error,
 *	on_exec_setup and on_exec_error.
 */
namespace boost { namespace process {

///Namespace containing the posix exensions.
namespace posix {


using ::boost::process::detail::posix::fd;
using ::boost::process::detail::posix::on_fork_error;
using ::boost::process::detail::posix::on_exec_setup;
using ::boost::process::detail::posix::on_exec_error;

}}}

#endif /* BOOST_PROCESS_POSIX_HPP_ */
