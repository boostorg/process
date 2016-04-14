// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_ARGS_HPP
#define BOOST_PROCESS_ARGS_HPP

#include <boost/process/detail/basic_cmd.hpp>


namespace boost { namespace process { namespace detail {



struct args_
{
    template <class Range>
    arg_setter_<Range, true>     operator()(Range &&range) const
    {
        return arg_setter_<Range, true>(std::move(range));
    }
    template <class Range>
    arg_setter_<Range, true>     operator+=(Range &&range) const
    {
        return arg_setter_<Range, true>(std::move(range));
    }
    template <class Range>
    arg_setter_<Range, false>    operator= (Range &&range) const
    {
        return arg_setter_<Range, true>(std::move>(range));
    }
    arg_setter_<std::string, true>     operator()(const char* str) const
    {
        return arg_setter_<std::string, true> (str);
    }
    arg_setter_<std::string, true>     operator+=(const char* str) const
    {
        return arg_setter_<std::string, true> (str);
    }
    arg_setter_<std::string, false>    operator= (const char* str) const
    {
        return arg_setter_<std::string, false>(str);
    }
    template<std::size_t Size>
    arg_setter_<std::string, true>     operator()(const char (&str) [Size]) const
    {
        return arg_setter_<std::string, true> (str);
    }
    template<std::size_t Size>
    arg_setter_<std::string, true>     operator+=(const char (&str) [Size]) const
    {
        return arg_setter_<std::string, true> (str);
    }
    template<std::size_t Size>
    arg_setter_<std::string, false>    operator= (const char (&str) [Size]) const
    {
        return arg_setter_<std::string, false>(str);
    }

    arg_setter_<std::string, true> operator()(std::initializer_list<const char*> &&range) const
    {
        return arg_setter_<std::string>(std::move(range));
    }
    arg_setter_<std::string, true> operator+=(std::initializer_list<const char*> &&range) const
    {
        return arg_setter_<std::string, true>(std::move(range));
    }
    arg_setter_<std::string, false> operator= (std::initializer_list<const char*> &&range) const
    {
        return arg_setter_<std::string, true>(std::move(range));
    }
    arg_setter_<std::string, true> operator()(std::initializer_list<std::string> &&range) const
    {
        return arg_setter_<std::string>(std::move(range));
    }
    arg_setter_<std::string, true> operator+=(std::initializer_list<std::string> &&range) const
    {
        return arg_setter_<std::string, true>(std::move(range));
    }
    arg_setter_<std::string, false> operator= (std::initializer_list<std::string> &&range) const
    {
        return arg_setter_<std::string, true>(std::move(range));
    }
};

}

constexpr boost::process::detail::args_ args{};
constexpr boost::process::detail::args_ argv{};


}}

#endif
