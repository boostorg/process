// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_TRAITS_DECL_HPP_
#define BOOST_PROCESS_DETAIL_TRAITS_DECL_HPP_

#include <boost/process/detail/config.hpp>
#include <boost/none.hpp>
#include <type_traits>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/handler.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/handler.hpp>
#endif


namespace boost { namespace process { namespace detail {


template<typename T>
using is_initializer = std::is_base_of<handler_base, T>;

template<typename T>
struct initializer_tag_t;

template<typename T>
using initializer_tag = typename initializer_tag_t<T>::type;


template<typename Tag>
struct make_initializer_t { typedef void type;}; //means I can disable the thingy

template<typename Tag, typename ...Args>
auto make_initializer(Args&&...args) -> typename make_initializer_t<Tag>::type
{
    return make_initializer_t<Tag>::invoke(std::forward<Args>(args));
}


template<typename First, typename ...Args>
struct valid_argument_list;

template<typename First>
struct valid_argument_list<First>
{
    constexpr bool value = is_initializer<First>::value || !std::is_void<intializer_tag<First>>::value;
    typedef std::integral_constant<bool, value> type;
};

template<typename First, typename ...Args>
struct valid_argument_list
{
    constexpr bool my_value = is_initializer<First>::value || !std::is_void<intializer_tag<First>>::value;
    constexpr bool value = valid_argument_list<Args...>::value && my_value;
    typedef std::integral_constant<bool, value> type;
};

}}}



#endif /* BOOST_PROCESS_DETAIL_HANDLER_HPP_ */
