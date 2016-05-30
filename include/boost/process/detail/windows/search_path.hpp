// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_SEARCH_PATH_HPP
#define BOOST_PROCESS_WINDOWS_SEARCH_PATH_HPP

#include <boost/process/detail/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/tokenizer.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include <stdexcept>
#include <array>
#include <atomic>
#include <cstdlib>
#include <boost/detail/winapi/shell.hpp>



namespace boost { namespace process { namespace detail { namespace windows {

inline std::string search_path(
        const std::string &filename,
        std::string path = "")
{
    if (path.empty())
    {
        path = ::getenv("PATH");
        if (path.empty())
            throw std::runtime_error(
                "Environment variable PATH not found");
    }

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(";");
    tokenizer tok(path, sep);
    for (boost::filesystem::path p : tok)
    {
        p /= filename;
        std::array<std::string, 4> extensions =
            { "", ".exe", ".com", ".bat" };
        for (boost::filesystem::path ext : extensions)
        {
            boost::filesystem::path p2 = p;
            p2 += ext;
            boost::system::error_code ec;
            bool file = boost::filesystem::is_regular_file(p2, ec);
            if (!ec && file &&
            	::boost::detail::winapi::sh_get_file_info(p2.string().c_str(), 0, 0, 0, ::boost::detail::winapi::SHGFI_EXETYPE_))
            {
                return p2.string();
            }
        }
    }
    return "";
}

}}}}

#endif
