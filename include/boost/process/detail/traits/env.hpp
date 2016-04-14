// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_ENV_HPP_
#define BOOST_PROCESS_DETAIL_ENV_HPP_

#include <boost/process/environment.hpp>
#include <boost/process/detail/traits/decl.hpp>


namespace boost { namespace process { namespace detail {

struct env_tag {};


template<typename T> struct env_set;
template<typename T> struct env_append;

struct env_reset;
struct env_init;

template<typename Char, template <class> class Implementation = detail::api::native_environment_impl>
class basic_environment;


template<class String> struct initializer_tag<env_set<String>>    { typedef env_tag type; };
template<class String> struct initializer_tag<env_append<String>> { typedef env_tag type; };

template<> struct initializer_tag<env_reset> { typedef env_tag type;};
template<> struct initializer_tag<env_init> { typedef env_tag type;};

template<typename Char, template <class> class Implementation>
struct initializer_tag<basic_environment<Char, Implementation>>   { typedef env_tag type; };


template<>
struct initializer_builder<env_tag>;

}


}}

#endif /* INCLUDE_BOOST_PROCESS_DETAIL_ENV_HPP_ */
