// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_PROC_INFO_HPP
#define BOOST_PROCESS_V2_DETAIL_PROC_INFO_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>

#include <boost/process/v2/pid.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)

#elif (defined(__APPLE__) && defined(__MACH__))
#include <string>
#include <vector>
#elif defined(__DragonFly__)
#include <sys/types.h>
#include <kvm.h>
#elif defined(__NetBSD__)
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(__OpenBSD__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#elif defined(__sun)
#include <kvm.h>
#include <fcntl.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

namespace ext 
{

#if defined(BOOST_PROCESS_V2_WINDOWS)
BOOST_PROCESS_V2_DECL std::wstring cwd_cmd_env_from_proc(HANDLE proc, int type, boost::system::error_code & ec);
BOOST_PROCESS_V2_DECL HANDLE open_process_with_debug_privilege(boost::process::v2::pid_type pid, boost::system::error_code & ec);
BOOST_PROCESS_V2_DECL BOOL is_x86_process(HANDLE proc, boost::system::error_code & ec);
#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)

#include <boost/process/v2/ext/detail/impl/proc_info.ipp>

#endif

#endif // BOOST_PROCESS_V2_DETAIL_PROC_INFO_HPP

