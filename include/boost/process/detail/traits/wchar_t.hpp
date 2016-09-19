// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_TRAITS_WCHAR_T_HPP_
#define BOOST_PROCESS_DETAIL_TRAITS_WCHAR_T_HPP_

#include <boost/process/detail/traits/decl.hpp>
#include <boost/process/detail/traits/cmd_or_exe.hpp>
#include <boost/process/detail/traits/env.hpp>

namespace boost { namespace process { namespace detail {

//template

template<typename T> struct is_wchar_t : std::false_type {};

template<> struct is_wchar_t<boost::filesystem::path>
{
    typedef typename
            std::is_same<typename boost::filesystem::path::value_type, wchar_t>::type type;
};

template<> struct is_wchar_t<const wchar_t* > : std::true_type {};

template<> struct is_wchar_t<wchar_t* > { typedef std::true_type type;};

template<std::size_t Size> struct is_wchar_t<const wchar_t [Size]>    : std::true_type {};
template<std::size_t Size> struct is_wchar_t<const wchar_t (&)[Size]> : std::true_type {};

template<> struct is_wchar_t<std::basic_string<wchar_t >>              : std::true_type {};
template<> struct is_wchar_t<std::vector<std::basic_string<wchar_t >>> : std::true_type {};
template<> struct is_wchar_t<std::initializer_list<std::basic_string<wchar_t >>> : std::true_type {};
template<> struct is_wchar_t<std::vector<wchar_t *>>           : std::true_type {};
template<> struct is_wchar_t<std::initializer_list<wchar_t *>> : std::true_type {};






}}}
#endif /* BOOST_PROCESS_DETAIL_TRAITS_WCHAR_T_HPP_ */
