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


/** \file boost/process/extend.hpp
 *
 * This header which provides the types and functions provided for custom extensions.
 *
 * \xmlonly
   Please refer to the <link linkend="boost_process.extend">tutorial</link> for more details.
   \endxmlonly
 */


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


///This handler is invoked before the process in launched, to setup parameters.
constexpr boost::process::detail::make_handler_t<boost::process::detail::on_setup_>   on_setup;
///This handler is invoked if an error occured.
constexpr boost::process::detail::make_handler_t<boost::process::detail::on_error_>   on_error;
///This handler is invoked if launching the process has succeeded.
constexpr boost::process::detail::make_handler_t<boost::process::detail::on_success_> on_success;

#if defined(BOOST_POSIX_API) || defined(BOOST_PROCESS_DOXYGEN)
///This handler is invoked if the fork failed. \note Only available on posix.
constexpr ::boost::process::detail::make_handler_t<::boost::process::detail::posix::on_fork_error_  >   on_fork_error;
///This handler is invoked if the fork succeeded. \note Only available on posix.
constexpr ::boost::process::detail::make_handler_t<::boost::process::detail::posix::on_exec_setup_  >   on_exec_setup;
///This handler is invoked if the exec call errored. \note Only available on posix.
constexpr ::boost::process::detail::make_handler_t<::boost::process::detail::posix::on_exec_error_     >   on_exec_error;
///@}
#endif

#if defined(BOOST_PROCESS_DOXYGEN)
/** This function gets the io_service from the initializer sequence.
 *
 * \attention Yields a compile-time error if no `io_service` is provided.
 * \param seq The Sequence of the initializer.
 */
template<typename Sequence>
inline asio::io_service& get_io_service(const Sequence & seq);

/** This class is the base for every initializer, to be used for extensions.
 *
 *  The usage is done through compile-time polymorphism, so that the required
 *  functions can be overloaded.
 *
 * \note None of the function need to be `const`.
 *
 */
struct handler
{
	///This function is invoked before the process launch. \note It is not required to be const.
    template <class Executor>
    void on_setup(Executor&) const {}

    /** This function is invoked if an error occured while trying to launch the process.
     * \note It is not required to be const.
     */
    template <class Executor>
    void on_error(Executor&, const std::error_code &) const {}

    /** This function is invoked if the process was successfully launched.
     * \note It is not required to be const.
     */
    template <class Executor>
    void on_success(Executor&) const {}

    /**This function is invoked if an error occured during the call of `fork`.
     * \note This function will only be called on posix.
     */
    template<typename Executor>
    void on_fork_error  (Executor &, const std::error_code&) const {}

    /**This function is invoked if the call of `fork` was successful, before
     * calling `execve`.
     * \note This function will only be called on posix.
     * \attention It will be invoked from the new process.
     */
    template<typename Executor>
    void on_exec_setup  (Executor &) const {}

    /**This function is invoked if the call of `execve` failed.
     * \note This function will only be called on posix.
     * \attention It will be invoked from the new process.
     */
    template<typename Executor>
    void on_exec_error  (Executor &, const std::error_code&) const {}

};


/** Inheriting the class will tell the launching process that an `io_service` is
 * needed. This should always be used when \ref get_io_service is used.
 *
 */
struct require_io_service {};
/** Inheriting this class will tell the launching function, that an event handler
 * shall be invoked when the process exits. This automatically does also inherit
 * \ref require_io_service.
 *
 * You must add the following function to your implementation:
 *
 \code{.cpp}
template<typename Executor>
std::function<void(int, const std::error_code&)> on_exit_handler(Executor & exec)
{
	auto handler = this->handler;
	return [handler](int exit_code, const std::error_code & ec)
		   {
				handler(static_cast<int>(exit_code), ec);
		   };

}
 \endcode

 The callback will be obtained by calling this function on setup and it will be
 invoked when the process exits.

 *
 * \warning Cannot be used with \ref boost::process::spawn
 */
struct async_handler : handler, require_io_service.
{

};
#endif

}
}
}

#endif
