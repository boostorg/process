// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/** \file boost/process/args.hpp
 *
 *    Header which provides the args property. It also provides the
 *    alternative name argv.
 */

#ifndef BOOST_PROCESS_ARGS_HPP
#define BOOST_PROCESS_ARGS_HPP

#include <boost/process/detail/basic_cmd.hpp>
#include <iterator>

namespace boost { namespace process { namespace detail {

struct args_
{
    template<typename T>
    using remove_reference_t = typename std::remove_reference<T>::type;
    template<typename T>
    using value_type = typename remove_reference_t<T>::value_type;

    template<typename T>
    using vvalue_type = value_type<value_type<T>>;

    template <class Range>
    arg_setter_<vvalue_type<Range>, true>     operator()(Range &&range) const
    {
        return arg_setter_<vvalue_type<Range>, true>(std::forward<Range>(range));
    }
    template <class Range>
    arg_setter_<vvalue_type<Range>, true>     operator+=(Range &&range) const
    {
        return arg_setter_<vvalue_type<Range>, true>(std::forward<Range>(range));
    }
    template <class Range>
    arg_setter_<vvalue_type<Range>, false>    operator= (Range &&range) const
    {
        return arg_setter_<vvalue_type<Range>, false>(std::forward<Range>(range));
    }
    template<typename Char>
    arg_setter_<Char, true>     operator()(std::basic_string<Char> && str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator+=(std::basic_string<Char> && str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, false>    operator= (std::basic_string<Char> && str) const
    {
        return arg_setter_<Char, false>(str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator()(const std::basic_string<Char> & str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator+=(const std::basic_string<Char> & str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, false>    operator= (const std::basic_string<Char> & str) const
    {
        return arg_setter_<Char, false>(str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator()(std::basic_string<Char> & str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator+=(std::basic_string<Char> & str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, false>    operator= (std::basic_string<Char> & str) const
    {
        return arg_setter_<Char, false>(str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator()(const Char* str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, true>     operator+=(const Char* str) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char>
    arg_setter_<Char, false>    operator= (const Char* str) const
    {
        return arg_setter_<Char, false>(str);
    }
    template<typename Char, std::size_t Size>
    arg_setter_<Char, true>     operator()(const Char (&str) [Size]) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char, std::size_t Size>
    arg_setter_<Char, true>     operator+=(const Char (&str) [Size]) const
    {
        return arg_setter_<Char, true> (str);
    }
    template<typename Char, std::size_t Size>
    arg_setter_<Char, false>    operator= (const Char (&str) [Size]) const
    {
        return arg_setter_<Char, false>(str);
    }

    template<typename Char>
    arg_setter_<Char, true> operator()(std::initializer_list<const Char*> &&range) const
    {
        return arg_setter_<Char>(range.begin(), range.end());
    }
    template<typename Char>
    arg_setter_<Char, true> operator+=(std::initializer_list<const Char*> &&range) const
    {
        return arg_setter_<Char, true>(range.begin(), range.end());
    }
    template<typename Char>
    arg_setter_<Char, false> operator= (std::initializer_list<const Char*> &&range) const
    {
        return arg_setter_<Char, true>(range.begin(), range.end());
    }
    template<typename Char>
    arg_setter_<Char, true> operator()(std::initializer_list<std::basic_string<Char>> &&range) const
    {
        return arg_setter_<Char>(range.begin(), range.end());
    }
    template<typename Char>
    arg_setter_<Char, true> operator+=(std::initializer_list<std::basic_string<Char>> &&range) const
    {
        return arg_setter_<Char, true>(range.begin(), range.end());
    }
    template<typename Char>
    arg_setter_<Char, false> operator= (std::initializer_list<std::basic_string<Char>> &&range) const
    {
        return arg_setter_<Char, true>(range.begin(), range.end());
    }
};

}

constexpr boost::process::detail::args_ args{};
constexpr boost::process::detail::args_ argv{};


}}

#endif
