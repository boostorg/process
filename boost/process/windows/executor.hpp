// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_EXECUTOR_HPP
#define BOOST_PROCESS_WINDOWS_EXECUTOR_HPP


#include <cstring>
#include <boost/process/windows/child.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/environment.hpp>

namespace boost { namespace process { namespace windows {

struct executor
{
    executor() : exe(0), cmd_line(0), proc_attrs(0), thread_attrs(0),
        inherit_handles(false),
#if (_WIN32_WINNT >= 0x0600)
        creation_flags(::boost::detail::winapi::extended_startupinfo_present),
#else
        creation_flags(0),
#endif
        env(0), work_dir(0)
#if (_WIN32_WINNT >= 0x0600)
        ,startup_info(startup_info_ex.StartupInfo)
#endif
    {
#if (_WIN32_WINNT >= 0x0600)
    	std::memset(&startup_info_ex, 0, sizeof(::boost::detail::winapi::STARTUPINFOEXW_));
        startup_info.cb = sizeof(::boost::detail::winapi::STARTUPINFOEXW_);
#else
        std::memset(&startup_info, 0, sizeof(::boost::detail::winapi::STARTUPINFOW_));
        startup_info.cb = sizeof(::boost::detail::winapi::STARTUPINFOW_);
#endif
        startup_info.hStdInput  = ::boost::detail::winapi::invalid_handle_value;
        startup_info.hStdOutput = ::boost::detail::winapi::invalid_handle_value;
        startup_info.hStdError  = ::boost::detail::winapi::invalid_handle_value;
    }

    struct call_on_CreateProcess_setup
    {
        executor &e_;

        call_on_CreateProcess_setup(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_CreateProcess_setup(e_);
        }
    };

    struct call_on_CreateProcess_error
    {
        executor &e_;

        call_on_CreateProcess_error(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_CreateProcess_error(e_);
        }
    };

    struct call_on_CreateProcess_success
    {
        executor &e_;

        call_on_CreateProcess_success(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_CreateProcess_success(e_);
        }
    };

    template <class InitializerSequence>
    child operator()(const InitializerSequence &seq)
    {
        boost::fusion::for_each(seq, call_on_CreateProcess_setup(*this));

        if (!::boost::detail::winapi::create_process(
            exe,
            cmd_line,
            proc_attrs,
            thread_attrs,
            inherit_handles,
            creation_flags,
            env,
            work_dir,
            &startup_info,
            &proc_info))
        {
            boost::fusion::for_each(seq, call_on_CreateProcess_error(*this));
        }
        else
        {
            boost::fusion::for_each(seq, call_on_CreateProcess_success(*this));
        }

        return child(proc_info);
    }

    const ::boost::detail::winapi::WCHAR_* exe;
    ::boost::detail::winapi::WCHAR_* cmd_line;
    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ proc_attrs;
    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ thread_attrs;
    ::boost::detail::winapi::BOOL_ inherit_handles;
    ::boost::detail::winapi::DWORD_ creation_flags;
    ::boost::detail::winapi::LPVOID_ env;
    const ::boost::detail::winapi::WCHAR_* work_dir;
#if (_WIN32_WINNT >= 0x0600)
    ::boost::detail::winapi::STARTUPINFOEX_ startup_info_ex;
    ::boost::detail::winapi::STARTUPINFOW_  &startup_info;
#else
    ::boost::detail::winapi::STARTUPINFOW_ startup_info;
#endif
    ::boost::detail::winapi::PROCESS_INFORMATION_ proc_info;
};

}}}

#endif
