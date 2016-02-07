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
#include <boost/process/windows/initializers/initializer_base.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

namespace boost { namespace process { namespace windows { namespace initializers {

template <class String>
struct start_in_dir_ : initializer_base
{
    typedef String string_type;
    typedef typename string_type::value_type char_type;
    explicit start_in_dir_(const String &s) : s_(s) {}

    template <class WindowsExecutor>
    void on_CreateProcess_setup(WindowsExecutor &e) const
    {
        e.work_dir = s_.c_str();
    }

private:
    String s_;
};

inline start_in_dir_<std::wstring> start_in_dir(const wchar_t *ws)
{
    return start_in_dir_<std::wstring>(ws);
}

inline start_in_dir_<std::wstring> start_in_dir(const std::wstring &ws)
{
    return start_in_dir_<std::wstring>(ws);
}

inline start_in_dir_<std::wstring> start_in_dir(const boost::filesystem::path &p)
{
    return start_in_dir_<std::wstring>(p.wstring());
}


#if !defined( BOOST_NO_ANSI_APIS )
inline start_in_dir_<std::string> start_in_dir(const char *s)
{
    return start_in_dir_<std::string>(s);
}

inline start_in_dir_<std::string> start_in_dir(const std::string &s)
{
    return start_in_dir_<std::string>(s);
}
/* Disabled, because boost::filesystem::path uses wchar_t internally.
inline start_in_dir_<std::string> start_in_dir(const boost::filesystem::path &p)
{
    return start_in_dir_<std::string>(p.string());
}*/
#endif //BOOST_NO_ANSI_APIS

}}}}

#endif
