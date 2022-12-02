// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_EXE_HPP
#define BOOST_PROCESS_V2_EXE_HPP

#include <boost/process/v2/detail/config.hpp>

#include <boost/process/v2/pid.hpp>

#include <system_error>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

// return executable path from pid
#ifdef BOOST_PROCESS_USE_STD_FS
BOOST_PROCESS_V2_DECL std::filesystem::path exe_path(boost::process::v2::pid_type pid, std::error_code & ec);
BOOST_PROCESS_V2_DECL std::filesystem::path exe_path(boost::process::v2::pid_type pid);
#else
BOOST_PROCESS_V2_DECL boost::filesystem::path exe_path(boost::process::v2::pid_type pid, std::error_code & ec);
BOOST_PROCESS_V2_DECL boost::filesystem::path exe_path(boost::process::v2::pid_type pid);
#endif

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_EXE_HPP

