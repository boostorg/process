// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/shell_path.hpp
 *
 * Defines a function to return the absolute path to a shell executable.
 */

#ifndef BOOST_PROCESS_SHELL_PATH_HPP
#define BOOST_PROCESS_SHELL_PATH_HPP

#include <boost/process/detail/config.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/shell_path.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/shell_path.hpp>
#endif

namespace boost { namespace process {

using boost::process::detail::api::shell_path;

}}

#if defined(BOOST_PROCESS_DOXYGEN)
namespace boost { namespace process {

/**
 * Returns the absolute path to a shell executable.
 *
 * \returns the path to cmd.exe on Windows and /bin/sh on POSIX.
 *
 * \throws std::system_error in case of an error
 */
boost::filesystem::path shell_path();

/**
 * Returns the absolute path to a shell executable.
 *
 * \returns the path to cmd.exe on Windows and /bin/sh on POSIX.
 */
boost::filesystem::path shell_path(std::error_code &ec);


}}
#endif


#endif
