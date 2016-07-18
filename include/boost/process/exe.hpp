// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_EXE_HPP
#define BOOST_PROCESS_EXE_HPP

#include <boost/process/detail/basic_cmd.hpp>

/** \file boost/process/exe.hpp
 *
 *    Header which provides the exe property.
 *
 */
namespace boost { namespace process { namespace detail {

struct exe_
{
    inline exe_setter_ operator()(const char *s) const
    {
        return exe_setter_(s);
    }
    inline exe_setter_ operator= (const char *s) const
    {
        return exe_setter_(s);
    }

    inline exe_setter_ operator()(const std::string &s) const
    {
        return exe_setter_(s);
    }
    inline exe_setter_ operator= (const std::string &s) const
    {
        return exe_setter_(s);
    }
};

}

constexpr boost::process::detail::exe_ exe{};

}}

#endif
