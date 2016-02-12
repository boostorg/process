// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_RUN_EXE_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_RUN_EXE_HPP

#include <boost/process/detail/initializers/base.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace boost { namespace process { namespace windows { namespace initializers {

template <class String>
struct run_exe_ : ::boost::process::detail::initializers::base
{
    typedef String string_type;
    typedef typename string_type::value_type char_type;
    explicit run_exe_(const String &s) : s_(s) {}

    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.exe = s_.c_str();
    }

private:
    String s_;
};

struct exe_
{
    inline run_exe_<std::wstring> operator()(const wchar_t *ws) const
    {
        return run_exe_<std::wstring>(ws);
    }
    inline run_exe_<std::wstring> operator= (const wchar_t *ws) const
    {
        return run_exe_<std::wstring>(ws);
    }

    inline run_exe_<std::wstring> operator()(const std::wstring &ws) const
    {
        return run_exe_<std::wstring>(ws);
    }
    inline run_exe_<std::wstring> operator= (const std::wstring &ws) const
    {
        return run_exe_<std::wstring>(ws);
    }

    inline run_exe_<std::wstring> operator()(const boost::filesystem::path &p) const
    {
        return run_exe_<std::wstring>(p.wstring());
    }
    inline run_exe_<std::wstring> operator= (const boost::filesystem::path &p) const
    {
        return run_exe_<std::wstring>(p.wstring());
    }

    #if !defined(BOOST_NO_ANSI_APIS)
    inline run_exe_<std::string> operator()(const char* p) const
    {
        return run_exe_<std::string>(p);
    }
    inline run_exe_<std::string> operator= (const char* p) const
    {
        return run_exe_<std::string>(p);
    }

    inline run_exe_<std::string> operator()(const std::string &s) const
    {
        return run_exe_<std::string>(s);
    }
    inline run_exe_<std::string> operator= (const std::string &s) const
    {
        return run_exe_<std::string>(s);
    }
};

constexpr static exe_ exe;
constexpr static exe_ cmd;


#endif // BOOST_NO_ANSI_APIS

}}}}

#endif
