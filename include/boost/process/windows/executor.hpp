// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_EXECUTOR_HPP
#define BOOST_PROCESS_WINDOWS_EXECUTOR_HPP


#include <cstring>
#include <boost/hana/for_each.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/process.hpp>
#include <system_error>


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

template<typename T>
using startup_info_t = typename startup_info<T>::type;

#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6

template<typename CharType> struct select_startup_info_ex;

#if !defined( BOOST_NO_ANSI_APIS )
template<> struct startup_info_ex<char>
{
    typedef ::boost::detail::winapi::STARTUPINFOEXA_ type;
};
#endif

template<> struct startup_info_ex<wchar_t>
{
    typedef ::boost::detail::winapi::STARTUPINFOEXW_ type;
};

template<typename T>
using startup_info_ex_t = typename startup_info_ex<T>::type;

#endif

#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6


template<typename CharT>
struct startup_info_impl
{
    ::boost::detail::winapi::DWORD_ creation_flags = ::boost::detail::winapi::extended_startupinfo_present;
    startup_info_ex_t<CharT>  startup_info_ex
            {nullptr,
        startup_info_t<CharT> {sizeof(startup_info_t<CharT>), nullptr, nullptr, nullptr,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr,
                               ::boost::detail::winapi::invalid_handle_value,
                               ::boost::detail::winapi::invalid_handle_value,
                               ::boost::detail::winapi::invalid_handle_value};
    }
    startup_info_t   <CharT> &startup_info = startup_info_ex.StartupInfo;
};


#else

template<typename CharT>
struct startup_info_impl
{
    ::boost::detail::winapi::DWORD_ creation_flags = 0;
    startup_info_t<CharT>           startup_info
            {sizeof(startup_info_t<CharT>), nullptr, nullptr, nullptr,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr,
             ::boost::detail::winapi::invalid_handle_value,
             ::boost::detail::winapi::invalid_handle_value,
             ::boost::detail::winapi::invalid_handle_value};

};

#endif

}




template<typename CharT>
struct basic_executor : detail::startup_info_impl<CharT>
{
    constexpr basic_executor() : detail::startup_info_impl<CharT>(),
            proc_attrs(0), thread_attrs(0), inherit_handles(false), work_dir(0)
    {
    }

    template<class Exe, class Env, class InitializerSequence>
    child operator()(const Exe & exe, const Env & env, const InitializerSequence &seq)
    {
        boost::hana::for_each(seq, [this](const auto &elem){elem.on_setup(*this);});
        int err_code = ::boost::detail::winapi::create_process(
            exe.get_exe(),
            exe.get_cmd_line(),
            proc_attrs,
            thread_attrs,
            inherit_handles,
            creation_flags,
            env.get(),
            work_dir,
            &startup_info,
            &proc_info);

        if (err_code == 0)
            boost::hana::for_each(seq,
                    [this, error](const auto &elem){elem.on_error(*this, boost::process::detail::get_last_error());});
        else
            boost::hana::for_each(seq, [this](const auto &elem){elem.on_success(*this);});

        return
                child(child_handle(std::move(proc_info)));
    }

    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ proc_attrs;
    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ thread_attrs;
    ::boost::detail::winapi::BOOL_ inherit_handles;
    const CharT* work_dir;

    ::boost::detail::winapi::PROCESS_INFORMATION_ proc_info;
};

#if !defined( BOOST_NO_ANSI_APIS )
typedef basic_executor<char>     executor;
#endif
typedef basic_executor<wchar_t> wexecutor;


}}}

#endif
