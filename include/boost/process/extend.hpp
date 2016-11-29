// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_EXTENSIONS_HPP_
#define BOOST_PROCESS_EXTENSIONS_HPP_

#include <boost/process/detail/handler.hpp>

#if defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/executor.hpp>
#include <boost/process/detail/windows/async_handler.hpp>
#else
#include <boost/process/detail/posix/executor.hpp>
#include <boost/process/detail/posix/async_handler.hpp>
#endif

namespace boost {
namespace process {
namespace asio {
class io_service;
}
namespace detail {
template<typename Tuple>
inline asio::io_service& get_io_service(const Tuple & tup);
}


///Namespace for extensions @attention This is experimental.
namespace extend {

#if defined(BOOST_WINDOWS_API)

template<typename ...Args>
using windows_executor = ::boost::process::detail::windows::executor<Args...>;
template<typename ...Args>
struct posix_executor;

#else

template<typename ...Args>
using posix_executor = ::boost::process::detail::posix::executor<Args...>;
template<typename ...Args>
struct windows_executor;

#endif

using ::boost::process::detail::handler;
using ::boost::process::detail::api::require_io_service;
using ::boost::process::detail::api::async_handler;
using ::boost::process::detail::get_io_service;

constexpr boost::process::detail::make_handler_t<boost::process::detail::on_setup_>   on_setup;
constexpr boost::process::detail::make_handler_t<boost::process::detail::on_error_>   on_error;
constexpr boost::process::detail::make_handler_t<boost::process::detail::on_success_> on_success;

#if defined(BOOST_POSIX_API)
///This handler is invoked if the fork fails.
constexpr static ::boost::process::detail::make_handler_t<::boost::process::detail::posix::on_fork_error_  >   on_fork_error;
///This handler is invoked if the fork succeeded.
constexpr static ::boost::process::detail::make_handler_t<::boost::process::detail::posix::on_exec_setup_  >   on_exec_setup;
///This handler is invoked if the exec call errored.
constexpr static ::boost::process::detail::make_handler_t<::boost::process::detail::posix::on_exec_error_     >   on_exec_error;
#endif

}
}
}

#endif
