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


#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/handler.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/handler.hpp>
#endif

#include <system_error>

#include <type_traits>
#include <boost/fusion/algorithm/query/find_if.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/sequence/comparison/equal_to.hpp>
#include <boost/fusion/container/set/convert.hpp>
#include <boost/type_index.hpp>

/** \file boost/process/error.hpp
 *
 *    Header which provides the error properties. It allows to explicitly set the error handling, the properties are:
 *
 *     - ignore_error
 *     - error
 *     - throw_on_error
 *
 *     For error there are two aliases: error_ref and error_code
 */

namespace boost { namespace process {

namespace detail {

struct throw_on_error_ : ::boost::process::detail::api::handler_base_ext
{
    constexpr throw_on_error_() {};

    template <class Executor>
    void on_error(Executor&, const std::error_code & ec) const
    {
        throw std::system_error(ec, "process creation failed");
    }

    const throw_on_error_ &operator()() const {return *this;}
};

struct ignore_error_ : ::boost::process::detail::api::handler_base_ext
{
    constexpr ignore_error_() {};
};

struct set_on_error : ::boost::process::detail::api::handler_base_ext
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
template<typename Sequence>
struct has_error_handler
{
    
    typedef typename boost::fusion::result_of::as_set<Sequence>::type set_type;
    typedef typename boost::fusion::result_of::has_key<set_type, set_on_error>::type            t1;
    typedef typename boost::fusion::result_of::has_key<set_type, set_on_error&>::type         t2;
    typedef typename boost::fusion::result_of::has_key<set_type, const set_on_error&>::type  t3;

    typedef typename boost::fusion::result_of::has_key<set_type, const throw_on_error_&>::type t4;
    typedef typename boost::fusion::result_of::has_key<set_type, const ignore_error_&>::type   t5;

    typedef typename boost::mpl::or_<t1,t2,t3, t4, t5>::type type;

};

template<typename Sequence>
struct has_ignore_error
{
    typedef typename boost::fusion::result_of::as_set<Sequence>::type set_type;
    typedef typename boost::fusion::result_of::has_key<set_type, ignore_error_>::type  type1;
    typedef typename boost::fusion::result_of::has_key<set_type, ignore_error_&>::type type2;
    typedef typename boost::fusion::result_of::has_key<set_type, const ignore_error_&>::type type3;
    typedef typename boost::mpl::or_<type1,type2, type3>::type type;

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
