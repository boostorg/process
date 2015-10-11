// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_SHELL_PATH_HPP
#define BOOST_PROCESS_WINDOWS_SHELL_PATH_HPP

#include <boost/process/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/GetSystemDirectory.hpp>

namespace boost { namespace process { namespace windows {

inline boost::filesystem::path shell_path()
{
	//260 is MAX_PATH but should be sufficient anyway.
    char sysdir[260];
    unsigned int size = ::boost::detail::winapi::GetSystemDirectory(sysdir, sizeof(sysdir));
    if (!size)
        BOOST_PROCESS_THROW_LAST_SYSTEM_ERROR("GetSystemDirectory() failed");
    boost::filesystem::path p = sysdir;
    return p / "cmd.exe";
}

inline boost::filesystem::path shell_path(boost::system::error_code &ec)
{
	char sysdir[260];
    unsigned int size = ::boost::detail::winapi::GetSystemDirectory(sysdir, sizeof(sysdir));
    boost::filesystem::path p;
    if (!size)
    {
        BOOST_PROCESS_RETURN_LAST_SYSTEM_ERROR(ec);
    }
    else
    {
        ec.clear();
        p = sysdir;
        p /= "cmd.exe";
    }
    return p;
}

}}}

#endif
