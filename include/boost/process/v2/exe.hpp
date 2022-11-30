// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_EXE_HPP
#define BOOST_PROCESS_V2_EXE_HPP

#include <boost/filesystem.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

#if defined(BOOST_PROCESS_V2_WINDOWS)
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#elif (defined(__APPLE__) && defined(__MACH__))
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#elif (defined(__linux__) || defined(__ANDROID__))
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#elif defined(__FreeBSD__) || defined(__DragonFly__)
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#elif defined(__NetBSD__)
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#elif defined(__OpenBSD__)
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#elif defined(__sun)
boost::filesystem::path exe_path(pid_type pid, error_code & ec);
#else
#error "Platform not supported"
#endif

} // namespace ext


BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_EXE_HPP

