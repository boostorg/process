// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_ARGS_HPP
#define BOOST_PROCESS_DETAIL_ARGS_HPP

#include <boost/algorithm/string/trim.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <sstream>
#include <initializer_list>
#include <iterator>

namespace boost { namespace process { namespace detail {

struct arg_setter_combiner;

template <class String, bool Append = false>
struct arg_setter_ : ::boost::process::detail::handler_base
{
    std::vector<String> _args;
    template<typename Range>
    arg_setter_(Range && str) :
            _args(std::make_move_iterator(str.begin()),
                  std::make_move_iterator(str.end())) {}
};


struct args_
{
    template <class Range>
    arg_setter_<Range, true>     operator()(Range &range) const
    {
        return arg_setter_<Range>(std::move(range));
    }
    template <class Range>
    arg_setter_<Range, true>     operator+=(Range &range) const
    {
        return arg_setter_<Range>(std::move(range));
    }
    template <class Range>
    arg_setter_<Range, false>    operator= (Range &range) const
    {
        return arg_setter_<Range>(std::move>(range));
    }
    template <class Char>
    arg_setter_<std::basic_string<Char>, true> operator()(std::initializer_list<Char*> &range) const
    {
        return arg_setter_<std::basic_string<Char>>(std::move(range));
    }
    template <class Char>
    arg_setter_<std::basic_string<Char>, true> operator+=(std::initializer_list<Char*> &range) const
    {
        return arg_setter_<std::basic_string<Char>, true>(std::move(range));
    }
    template <class Char>
    arg_setter_<std::basic_string<Char>, false> operator= (std::initializer_list<Char*> &range) const
    {
        return arg_setter_<std::basic_string<Char>, true>(std::move(range));
    }
};


template<class Char, bool Append>
inline constexpr std:: true_type is_arg_setter(const arg_setter_<Char, Append>&) {return {};}

template<typename T>
inline constexpr std::false_type is_arg_setter(const T &) {return {};}

template<typename T>
struct is_arg_setter_t : decltype(is_arg_setter(T())) {};


}

constexpr boost::process::detail::args_ args{};
constexpr boost::process::detail::args_ argv{};


}}

#endif
