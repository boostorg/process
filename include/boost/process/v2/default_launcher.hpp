//
// boost/process/v2/default_launcher.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Klemens D. Morgenstern (klemens dot morgenstern at gmx dot net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_V2_DEFAULT_LAUNCHER_HPP
#define BOOST_PROCESS_V2_DEFAULT_LAUNCHER_HPP

#include <boost/process/v2/detail/config.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <boost/process/v2/windows/default_launcher.hpp>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_V2_WINDOWS)
typedef windows::default_launcher default_process_launcher;
#else


#endif


BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)
#include <boost/process/v2/impl/default_launcher.ipp>
#endif

#endif //BOOST_PROCESS_V2_DEFAULT_LAUNCHER_HPP