// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_CMD_LINE_HPP
#define BOOST_PROCESS_DETAIL_CMD_LINE_HPP

#include <boost/detail/winapi/config.hpp>
#include <boost/process/detail/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/traits/cmd_or_exe.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/cmd.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/cmd.hpp>
#endif


namespace boost { namespace process { namespace detail {


struct cmd_
{
    constexpr cmd_() {}

    inline api::cmd_setter_ operator()(const char *s) const
    {
        return api::cmd_setter_(s);
    }
    inline api::cmd_setter_ operator= (const char *s) const
    {
        return api::cmd_setter_(s);
    }

    inline api::cmd_setter_ operator()(const std::string &s) const
    {
        return api::cmd_setter_(s);
    }
    inline api::cmd_setter_ operator= (const std::string &s) const
    {
        return api::cmd_setter_(s);
    }
};

constexpr static cmd_ cmd;

}

using boost::process::detail::cmd;

}}

#endif
