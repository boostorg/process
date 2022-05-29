// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_PROCESS_HANDLE_HPP
#define BOOST_PROCESS_V2_PROCESS_HANDLE_HPP

#include <boost/process/v2/detail/config.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <boost/process/v2/detail/process_handle_windows.hpp>
#else

#if defined(BOOST_PROCESS_V2_PIDFD_OPEN)
#include <boost/process/v2/detail/process_handle_fd.hpp>
#elif defined(BOOST_PROCESS_V2_PDFORK)
#include <boost/process/v2/detail/process_handle_fd_or_signal.hpp>
#else
// with asio support we could use EVFILT_PROC:NOTE_EXIT as well.
#include <boost/process/v2/detail/process_handle_signal.hpp>
#endif
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_V2_WINDOWS)
template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>
using basic_process_handle = detail::basic_process_handle_win<Executor>;
#else

#if defined(BOOST_PROCESS_V2_PIDFD_OPEN)
template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>
using basic_process_handle = detail::basic_process_handle_fd<Executor>;
#elif defined(BOOST_PROCESS_V2_PDFORK)
template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>
using basic_process_handle = detail::basic_process_handle_fd_or_signal<Executor>;
#else

template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>
using basic_process_handle = detail::basic_process_handle_signal<Executor>;

#endif

using process_handle = basic_process_handle<>;

#endif

BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)

#include <boost/process/v2/impl/process_handle.ipp>

#endif




#endif //BOOST_PROCESS_V2_PROCESS_HANDLE_HPP
