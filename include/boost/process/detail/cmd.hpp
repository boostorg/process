// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_CMD_LINE_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_CMD_LINE_HPP

#include <boost/detail/winapi/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/shared_array.hpp>
#include <memory>

namespace boost { namespace process { namespace detail {

template <class String>
struct cmd_setter_ : ::boost::process::detail::initializers::handler_base
{
    typedef String string_type;
    typedef typename String::value_type char_type;

    explicit cmd_setter_(const string_type &s) : cmd_line_(s) {}
    explicit cmd_setter_(string_type &&s     ) : cmd_line_(std::move(s)) {}



private:
    string_type cmd_line_;
};

template<template <class> class CmdSetter>
struct cmd_
{
    inline CmdSetter<std::wstring> operator()(const wchar_t *ws) const
    {
        return CmdSetter<std::wstring>(ws);
    }
    inline CmdSetter<std::wstring> operator= (const wchar_t *ws) const
    {
        return CmdSetter<std::wstring>(ws);
    }

    inline CmdSetter<std::wstring> operator()(const std::wstring &ws) const
    {
        return CmdSetter<std::wstring>(ws);
    }
    inline CmdSetter<std::wstring> operator= (const std::wstring &ws) const
    {
        return CmdSetter<std::wstring>(ws);
    }
    #if !defined( BOOST_NO_ANSI_APIS )
    inline CmdSetter<std::string> operator()(const char *s) const
    {
        return CmdSetter<std::string>(s);
    }
    inline CmdSetter<std::string> operator= (const char *s) const
    {
        return CmdSetter<std::string>(s);
    }

    inline CmdSetter<std::string> operator()(const std::string &s) const
    {
        return CmdSetter<std::string>(s);
    }
    inline CmdSetter<std::string> operator= (const std::string &s) const
    {
        return CmdSetter<std::string>(s);
    }
    #endif //BOOST_NO_ANSI_APIS
    };

constexpr static cmd_ cmd;
constexpr static cmd_ set_cmd;

}}}

#endif
