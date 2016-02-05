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

namespace boost { namespace process { namespace windows {

namespace detail
{
template<typename CharType> struct startup_info;
#if !defined( BOOST_NO_ANSI_APIS )

template<> struct startup_info<char>
{
    typedef ::boost::detail::winapi::STARTUPINFOA_ type;
};
#endif

template<> struct startup_info<wchar_t>
{
    typedef ::boost::detail::winapi::STARTUPINFOW_ type;
};
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6

template<typename CharType> struct select_startup_info_ex;

#if !defined( BOOST_NO_ANSI_APIS )
template<> struct startup_info_ex<char>
{
    typedef ::boost::detail::winapi::STARTUPINFOEXA_ type;
};
#endif

template<> struct sstartup_info_ex<wchar_t>
{
    typedef ::boost::detail::winapi::STARTUPINFOEXW_ type;
};


#endif
}

template<typename CharT>
struct basic_executor
{
    //define the underlying startupinfo structures. THis is done here, to avoid c++11 using.
    typedef typename ::boost::process::windows::detail::startup_info<CharT>::type    startup_info_t;
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
    typedef typename ::boost::process::windows::detail::startup_info_ex<CharT>::type startup_info_ex_t;
#endif

    basic_executor() : exe(0), cmd_line(0), proc_attrs(0), thread_attrs(0),
        inherit_handles(false),
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
        creation_flags(::boost::detail::winapi::extended_startupinfo_present),
#else
        creation_flags(0),
#endif
        env(0), work_dir(0)
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
        ,startup_info(startup_info_ex.StartupInfo)
#endif
    {
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
    	std::memset(&startup_info_ex, 0, sizeof(startup_info_ex_t));
        startup_info.cb = sizeof(startup_info_ex_t);
#else
        std::memset(&startup_info, 0, sizeof(startup_info_t));
        startup_info.cb = sizeof(startup_info_t);
#endif
        startup_info.hStdInput  = ::boost::detail::winapi::invalid_handle_value;
        startup_info.hStdOutput = ::boost::detail::winapi::invalid_handle_value;
        startup_info.hStdError  = ::boost::detail::winapi::invalid_handle_value;
    }

    struct call_on_CreateProcess_setup
    {
        basic_executor &e_;

        call_on_CreateProcess_setup(basic_executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_CreateProcess_setup(e_);
        }
    };

    struct call_on_CreateProcess_error
    {
        basic_executor &e_;

        call_on_CreateProcess_error(basic_executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_CreateProcess_error(e_);
        }
    };

    struct call_on_CreateProcess_success
    {
        basic_executor &e_;

        call_on_CreateProcess_success(basic_executor &e) : e_(e) {}

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

    const CharT* exe;
    CharT* cmd_line;
    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ proc_attrs;
    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ thread_attrs;
    ::boost::detail::winapi::BOOL_ inherit_handles;
    ::boost::detail::winapi::DWORD_ creation_flags;
    ::boost::detail::winapi::LPVOID_ env;
    const CharT* work_dir;
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
    startup_info_ex_t startup_info_ex;
    startup_info_t   &startup_info;
#else
    startup_info_t startup_info;
#endif
    ::boost::detail::winapi::PROCESS_INFORMATION_ proc_info;
};

#if !defined( BOOST_NO_ANSI_APIS )
typedef basic_executor<char>     executor;
#endif
typedef basic_executor<wchar_t> wexecutor;


}}}

#endif
