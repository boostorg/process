// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_START_IN_DIR_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_START_IN_DIR_HPP

#include <boost/detail/winapi/config.hpp>
#include <boost/process/detail/initializers/base.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

namespace boost { namespace process { namespace windows { namespace initializers {

template <class String>
struct start_dir_handler : ::boost::process::detail::initializers::base
{
    typedef String string_type;
    typedef typename string_type::value_type char_type;
    explicit start_dir_handler(const String &s) : s_(s) {}

    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.work_dir = s_.c_str();
    }

private:
    String s_;
};

struct start_dir_
{
    start_dir_handler<std::wstring> operator()(const wchar_t *ws) const
    {
        return start_dir_handler<std::wstring>(ws);
    }
    start_dir_handler<std::wstring> operator= (const wchar_t *ws) const
    {
        return start_dir_handler<std::wstring>(ws);
    }

    start_dir_handler<std::wstring> operator()(const std::wstring &ws) const
    {
        return start_dir_handler<std::wstring>(ws);
    }
    start_dir_handler<std::wstring> operator= (const std::wstring &ws) const
    {
        return start_dir_handler<std::wstring>(ws);
    }
    start_dir_handler<std::wstring> operator()(const boost::filesystem::path &s) const
    {
        return start_dir_handler<std::wstring>(s);
    }
    start_dir_handler<std::wstring> operator= (const boost::filesystem::path &s) const
    {
        return start_dir_handler<std::wstring>(s);
    }

#if !defined( BOOST_NO_ANSI_APIS )
    start_dir_handler<std::string> operator()(const char *ws) const
    {
        return start_dir_handler<std::string>(ws);
    }
    start_dir_handler<std::string> operator= (const char *ws) const
    {
        return start_dir_handler<std::string>(ws);
    }
    start_dir_handler<std::string> operator()(const std::string &s) const
    {
        return start_dir_handler<std::string>(s);
    }
    start_dir_handler<std::string> operator= (const std::string &s) const
    {
        return start_dir_handler<std::string>(s);
    }
#endif //BOOST_NO_ANSI_APIS
};

constexpr start_dir_ start_dir;

}}}}

#endif
