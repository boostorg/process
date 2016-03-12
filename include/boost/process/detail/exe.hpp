// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_EXE_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_EXE_HPP

#include <boost/detail/winapi/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/shared_array.hpp>
#include <memory>

namespace boost { namespace process { namespace detail {

template <class String>
struct exe_setter_ : ::boost::process::detail::handler_base
{
    String exe_;
    exe_setter_(String && str) : exe_(std::move(str)) {}
};

template<template <class> class ExeSetter>
struct exe_
{
    inline ExeSetter<std::wstring> operator()(const wchar_t *ws) const
    {
        return ExeSetter<std::wstring>(ws);
    }
    inline ExeSetter<std::wstring> operator= (const wchar_t *ws) const
    {
        return ExeSetter<std::wstring>(ws);
    }

    inline ExeSetter<std::wstring> operator()(const std::wstring &ws) const
    {
        return ExeSetter<std::wstring>(ws);
    }
    inline ExeSetter<std::wstring> operator= (const std::wstring &ws) const
    {
        return ExeSetter<std::wstring>(ws);
    }
    inline ExeSetter<std::string> operator()(const char *s) const
    {
        return ExeSetter<std::string>(s);
    }
    inline ExeSetter<std::string> operator= (const char *s) const
    {
        return ExeSetter<std::string>(s);
    }

    inline ExeSetter<std::string> operator()(const std::string &s) const
    {
        return ExeSetter<std::string>(s);
    }
    inline ExeSetter<std::string> operator= (const std::string &s) const
    {
        return ExeSetter<std::string>(s);
    }
};

template<class String>
inline constexpr std:: true_type is_exe_setter(const exe_setter_<String>&) {return {};}
template<typename T>
inline constexpr std::false_type is_exe_setter(const T &) {return {};}

template<typename T>
struct is_exe_setter_t : decltype(is_exe_setter(T())) {};



}}}

#endif
