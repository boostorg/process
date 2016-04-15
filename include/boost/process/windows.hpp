// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_WINDOWS_HPP_
#define BOOST_PROCESS_WINDOWS_HPP_

#include <boost/process/detail/windows/show_window.hpp>

namespace boost { namespace process {


using boost::process::detail::windows::hide;
using boost::process::detail::windows::maximized;
using boost::process::detail::windows::minimized;
using boost::process::detail::windows::minimized_not_active;
using boost::process::detail::windows::not_active;
using boost::process::detail::windows::show;
using boost::process::detail::windows::show_normal;


}}

#endif /* BOOST_PROCESS_WINDOWS_HPP_ */
