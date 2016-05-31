// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
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

/** \file boost/process/shell.hpp
 *
 *	Header which provides the shell property. This provides the
 *	property to launch a process through the system shell.
 *	It also allows the user to obtain the shell-path via shell().
 */

namespace boost { namespace process { namespace detail {


struct shell_
{
    constexpr shell_() {}

    boost::filesystem::path operator()() const
    {
        return boost::process::detail::api::shell_path();
    }
    boost::filesystem::path operator()(std::error_code & ec) const noexcept
    {
        return boost::process::detail::api::shell_path(ec);
    }
};

}

constexpr static ::boost::process::detail::shell_ shell;

}}



#endif
