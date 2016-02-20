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

template <class Char, bool Append = false>
struct arg_setter_ : ::boost::process::detail::handler
{
    typedef typename Char char_type;
    typedef std::basic_stringstream<char_type>   string_type;
    typedef typename string_type::const_iterator const_iterator;

    constexpr static char_type quote_sign = _get_quote_sign(char_type());
    constexpr static char_type space_sign = _get_space_sign(char_type());

    constexpr static bool store_after_launch = false;//
    constexpr static bool is_append = Append;

    arg_setter_(arg_setter_&&) = default;
    arg_setter_& operator=(arg_setter_&&) = default;
    explicit arg_setter_(const std::vector<std::string> & args) : _args(args)  {}
    explicit arg_setter_(      std::vector<std::string> &&args) : _args(args) {};

    static constexpr  char   _get_space_sign(char)    {return  ' ';}
    static constexpr wchar_t _get_space_sign(wchar_t) {return L' ';}
    static constexpr  char   _get_quote_sign(char)    {return  '"';}
    static constexpr wchar_t _get_quote_sign(wchar_t) {return L'"';}

    template <class Executor>
    void on_setup(Executor& e) const
    {
        api::apply_args(_args, e);
    }

private:
    friend arg_setter_combiner;
    std::vector<string_type> _args;
};


struct arg_setter_combiner
{
    template<typename T, bool B0>
    static void combine(arg_setter_<T, B0> & base, const arg_setter_<T, true> &  rhs)
    {
        base._args.insert(base._args.end(), rhs._args.begin(), rhs._args.end());
    }

    template<typename T, bool B0>
    static void combine(arg_setter_<T, B0> & base,       arg_setter_<T, true> && rhs)
    {
        base._args.insert(base._args.end(),
                            std::move_iterator(rhs._args.begin()),
                            std::move_iterator(rhs._args.end()  ));
    }
    template<typename T, bool B0>
    static void combine(arg_setter_<T, B0> & base, const arg_setter_<T, false> &  rhs)
    {
        base._args = rhs._args;
    }

    template<typename T, bool B0>
    static void combine(arg_setter_<T, B0> & base,       arg_setter_<T, false> && rhs)
    {
        base._args = std::move(rhs._args);
    }
};

template<typename T, bool B0, bool B1>
void combine(arg_setter_<T, B0> & base, const arg_setter_<T, B1> &  rhs) {arg_setter_combiner::combine(base, rhs);}
template<typename T, bool B0, bool B1>
void combine(arg_setter_<T, B0> & base,       arg_setter_<T, B1> && rhs) {arg_setter_combiner::combine(base, std::move(rhs));}

struct args_
{
    template <class Range>
    arg_setter_<Range, true>     operator()(Range &&range) const
    {
        return Template<Range>(std::forward<Range>(range));
    }
    template <class Range>
    arg_setter_<Range, true>     operator+=(Range &&range) const
    {
        return Template<Range>(std::forward<Range>(range));
    }
    template <class Range>
    arg_setter_<Range, false>    operator= (Range &&range) const
    {
        return Template<Range>(std::forward<Range>(range));
    }
    template <class Char>
    arg_setter_<Char, true>      operator()(const std::initializer_list<Char*> &range) const
    {
        return Template<Char>(range);
    }
    template <class Char>
    arg_setter_<Char, true>      operator+=(const std::initializer_list<Char*> &range) const
    {
        return Template<Char, true>(range);
    }
    template <class Char>
    arg_setter_<Char, false>     operator= (const std::initializer_list<Char*> &range) const
    {
        return Template<Char, true>(range);
    }
};

}

constexpr boost::process::detail::args_ args;
constexpr boost::process::detail::args_ argv;


}}

#endif
