// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_ASYNC_HPP_
#define BOOST_PROCESS_ASYNC_HPP_

#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/traits.hpp>
#include <boost/process/detail/windows/async_handler.hpp>
#include <boost/type_index.hpp>
#include <boost/asio.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/on_exit.hpp>
#include <boost/process/detail/posix/io_service_ref.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/on_exit.hpp>
#include <boost/process/detail/windows/io_service_ref.hpp>
#endif


namespace boost { namespace process { namespace detail {





}}}

#endif /* INCLUDE_BOOST_ASYNC_HPP_ */
