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
struct cmd_setter_ : ::boost::process::detail::handler
{
    typedef String string_type;
    typedef typename String::value_type char_type;

    explicit cmd_setter_(const string_type &s) : _cmd_line(s) {}
    explicit cmd_setter_(string_type &&s     ) : _cmd_line(std::move(s)) {}

    template <class Executor>
    void on_setup(Executor &e) const
    {
        api::apply_cmd(_cmd_line, e);
    }

private:
    string_type _cmd_line;
};

struct cmd_
{
    inline cmd_setter_<std::wstring> operator()(const wchar_t *ws) const
    {
        return cmd_setter_<std::wstring>(ws);
    }
    inline cmd_setter_<std::wstring> operator= (const wchar_t *ws) const
    {
        return cmd_setter_<std::wstring>(ws);
    }

    inline cmd_setter_<std::wstring> operator()(const std::wstring &ws) const
    {
        return cmd_setter_<std::wstring>(ws);
    }
    inline cmd_setter_<std::wstring> operator= (const std::wstring &ws) const
    {
        return cmd_setter_<std::wstring>(ws);
    }
    inline cmd_setter_<std::string> operator()(const char *s) const
    {
        return cmd_setter_<std::string>(s);
    }
    inline cmd_setter_<std::string> operator= (const char *s) const
    {
        return cmd_setter_<std::string>(s);
    }

    inline cmd_setter_<std::string> operator()(const std::string &s) const
    {
        return cmd_setter_<std::string>(s);
    }
    inline cmd_setter_<std::string> operator= (const std::string &s) const
    {
        return cmd_setter_<std::string>(s);
    }
};


template<class String>
inline constexpr std:: true_type is_cmd_setter(const cmd_setter_<String>&) {return {};}
inline constexpr std::false_type is_cmd_setter(const auto &) {return {};}


}}}

#endif
