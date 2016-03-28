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

#include <boost/process/child.hpp>
#include <boost/process/detail/traits.hpp>
#include <cstring>
#include <boost/hana/for_each.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/transform.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/none.hpp>
#include <system_error>
#include <memory>

#include <boost/type_index.hpp>

namespace boost { namespace process { namespace detail { namespace windows {

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




template<typename Sequence>
struct executor : startup_info_impl<char>
{
    executor(Sequence & seq) : seq(seq)
    {
    }

    void internal_throw(std:: true_type, std::error_code &ec ) {}
    void internal_throw(std::false_type, std::error_code &ec )
    {
        throw std::system_error(ec);
    }


    child operator()()
    {
        boost::hana::for_each(seq, [this](const auto &elem) {elem->on_setup(*this);});

        //NOTE: The non-cast cmd-line string can only be modified by the wchar_t variant which is currently disabled.
        int err_code = ::boost::detail::winapi::create_process(
            exe,                                        //       LPCSTR_ lpApplicationName,
            const_cast<char*>(cmd_line),                //       LPSTR_ lpCommandLine,
            proc_attrs,                                 //       LPSECURITY_ATTRIBUTES_ lpProcessAttributes,
            thread_attrs,                               //       LPSECURITY_ATTRIBUTES_ lpThreadAttributes,
            inherit_handles,                            //       INT_ bInheritHandles,
            creation_flags,                             //       DWORD_ dwCreationFlags,
            reinterpret_cast<void*>(const_cast<char*>(env)),  //     LPVOID_ lpEnvironment,
            work_dir,                                   //       LPCSTR_ lpCurrentDirectory,
            &this->startup_info,                        //       LPSTARTUPINFOA_ lpStartupInfo,
            &proc_info);                                //       LPPROCESS_INFORMATION_ lpProcessInformation)

        if (err_code == 0)
        {
            auto last_error = boost::process::detail::get_last_error();
            boost::hana::for_each(seq,
                    [this, err_code, last_error](const auto &elem){elem->on_error(*this, last_error);});
            internal_throw(has_error_handler(seq), last_error);
        }
        else
            boost::hana::for_each(seq, [this](const auto &elem){elem->on_success(*this);});
        return
                child(child_handle(std::move(proc_info)), make_resource_pointer(seq));
    }

    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ proc_attrs   = nullptr;
    ::boost::detail::winapi::LPSECURITY_ATTRIBUTES_ thread_attrs = nullptr;
    ::boost::detail::winapi::BOOL_ inherit_handles = false;
    const char * work_dir = nullptr;
    const char * cmd_line       = nullptr;
    const char * exe      = nullptr;
    const char * env      = nullptr;

    ::boost::detail::winapi::PROCESS_INFORMATION_ proc_info;

    Sequence & seq;
};



template<typename Tup>
executor<Tup> make_executor(Tup & tup)
{
    return executor<Tup>(tup);
}


}}}}

#endif
