// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_EXE_HPP
#define BOOST_PROCESS_V2_DETAIL_EXE_HPP

#include <boost/process/v2/pid.hpp>

#include <system_error>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

namespace ext 
{

#if defined(BOOST_PROCESS_V2_WINDOWS)

BOOST_PROCESS_V2_DECL HANDLE open_process_with_debug_privilege(pid_type pid, error_code & ec);
BOOST_PROCESS_V2_DECL BOOL is_x86_process(HANDLE proc, error_code & ec);

#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#endif BOOST_PROCESS_V2_DETAIL_EXE_HPP

