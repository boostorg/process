// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_ERROR_HPP
#define BOOST_PROCESS_DETAIL_ERROR_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/traits.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <system_error>

#include <type_traits>
#include <boost/fusion/algorithm/query/find_if.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>

#include <boost/type_index.hpp>
#include <iostream>

namespace boost { namespace process {

namespace detail {

struct throw_on_error_ : ::boost::process::detail::handler_base
{
    constexpr throw_on_error_() {};

    template <class Executor>
    void on_error(Executor&, const std::error_code & ec) const
    {
        throw std::system_error(ec, "process creation failed");
    }

    const throw_on_error_ &operator()() const {return *this;}
};

struct ignore_error_ : ::boost::process::detail::handler_base
{
    constexpr ignore_error_() {};
};

struct set_on_error : ::boost::process::detail::handler_base
{
    set_on_error(const set_on_error&) = default;
    explicit set_on_error(std::error_code &ec) : ec_(ec) {}

    template <class Executor>
    void on_error(Executor&, const std::error_code & ec) const
    {
        ec_ = ec;
    }

private:
    std::error_code &ec_;
};

struct error_
{
    constexpr error_() {}
    set_on_error operator()(std::error_code &ec) const {return set_on_error(ec);}
    set_on_error operator= (std::error_code &ec) const {return set_on_error(ec);}

};


template<typename T>
struct is_error_handler : std::false_type {};

template<> struct is_error_handler<set_on_error>    : std::true_type {};
template<> struct is_error_handler<throw_on_error_> : std::true_type {};
template<> struct is_error_handler<ignore_error_>   : std::true_type {};


//note: is a tuple of pointers to initializers
template<typename Tuple>
struct has_error_handler
{
    typedef typename boost::fusion::result_of::find_if<
            Tuple,
            is_error_handler<typename std::remove_reference<boost::mpl::_>::type>
                >::type itr;

    typedef typename boost::fusion::result_of::end<Tuple>::type end;
    typedef typename boost::fusion::result_of::equal_to<itr, end>::type equal;
    typedef std::integral_constant<bool,
            !equal::value > type;

};

struct error_builder
{
    std::error_code *err;
    typedef set_on_error result_type;
    set_on_error get_initializer() {return set_on_error(*err);};
    void operator()(std::error_code & ec) {err = &ec;};
};

template<>
struct initializer_tag<std::error_code>
{
    typedef error_tag type;
};


template<>
struct initializer_builder<error_tag>
{
    typedef error_builder type;
};

}

constexpr static boost::process::detail::ignore_error_ ignore_error;
constexpr static boost::process::detail::error_ error;
constexpr static boost::process::detail::error_ error_ref;
constexpr static boost::process::detail::error_ error_code;
constexpr static boost::process::detail::throw_on_error_ throw_on_error;


}}

#endif
