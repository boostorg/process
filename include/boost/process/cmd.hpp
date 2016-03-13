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
#include <boost/process/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/cmd_or_exe.hpp>
#include <boost/process/windows/cmd.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/shared_array.hpp>
#include <memory>
#include <boost/hana/tuple.hpp>

namespace boost { namespace process { namespace detail {

template <class String>
struct cmd_setter_ : ::boost::process::detail::handler_base
{
    cmd_setter_(String && cmd_line) : _cmd_line(std::move(cmd_line)) {}
    cmd_setter_(const String & cmd_line) : _cmd_line(cmd_line) {}
    template <class Executor>
    void on_setup(Executor& exec) const
    {
        detail::api::apply_cmd(_cmd_line, exec);
    }
public:
    String _cmd_line;
};

struct cmd_
{
    inline cmd_setter_<const wchar_t *> operator()(const wchar_t *ws) const
    {
        return cmd_setter_<const wchar_t *>(ws);
    }
    inline cmd_setter_<const wchar_t *> operator= (const wchar_t *ws) const
    {
        return cmd_setter_<const wchar_t *>(ws);
    }

    inline cmd_setter_<std::wstring> operator()(const std::wstring &ws) const
    {
        return cmd_setter_<std::wstring>(ws);
    }
    inline cmd_setter_<std::wstring> operator= (const std::wstring &ws) const
    {
        return cmd_setter_<std::wstring>(ws);
    }
    inline cmd_setter_<const char *> operator()(const char *s) const
    {
        return cmd_setter_<const char *>(s);
    }
    inline cmd_setter_<const char *> operator= (const char *s) const
    {
        return cmd_setter_<const char *>(s);
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

template<typename T>
std::true_type is_initializer(const cmd_setter_<T> &){return {};}

template<std::size_t Size>
inline cmd_setter_<const char *> make_initializer(cmd_or_exe_tag&, boost::hana::tuple<char (*)[Size]> & cmd)
{
    const auto & value = *boost::hana::at(cmd, boost::hana::size_c<0>);
    return cmd_setter_<const char *>(value);
}

template<std::size_t Size>
inline cmd_setter_<const char *> make_initializer(cmd_or_exe_tag&, boost::hana::tuple<const char (*)[Size]> & cmd)
{
    const auto & value = *boost::hana::at(cmd, boost::hana::size_c<0>);
    return cmd_setter_<const char *>(value);
}

inline cmd_setter_<const char *> make_initializer(cmd_or_exe_tag&, boost::hana::tuple<char **> & cmd)
{
    const auto & value = *boost::hana::at(cmd, boost::hana::size_c<0>);
    return cmd_setter_<const char *>(value);
}
inline cmd_setter_<const char *> make_initializer(cmd_or_exe_tag&, boost::hana::tuple<const char **> & cmd)
{
    const auto & value = *boost::hana::at(cmd, boost::hana::size_c<0>);
    return cmd_setter_<const char *>(value);
}
inline cmd_setter_<std::string> make_initializer (cmd_or_exe_tag&, boost::hana::tuple<std::string*> &cmd)
{
    auto && value = std::move(*boost::hana::at(cmd, boost::hana::size_c<0>));
    return cmd_setter_<std::string>(value);
}

}}}

#endif
