// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_CMD_HPP
#define BOOST_PROCESS_V2_CMD_HPP

#include <string>
#include <vector>

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>

#include <boost/process/v2/pid.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

// return commandline from pid
BOOST_PROCESS_V2_DECL std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec);
BOOST_PROCESS_V2_DECL std::vector<std::string> commandline(boost::process::v2::pid_type pid);

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_CMD_HPP
