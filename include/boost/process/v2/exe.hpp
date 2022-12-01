// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_EXE_HPP
#define BOOST_PROCESS_V2_EXE_HPP

#include <boost/process/filesystem.hpp>

#include <system_error>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

BOOST_PROCESS_V2_DECL filesystem::path exe_path(pid_type pid, error_code & ec);
BOOST_PROCESS_V2_DECL filesystem::path exe_path(pid_type pid);

} // namespace ext


BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_EXE_HPP

