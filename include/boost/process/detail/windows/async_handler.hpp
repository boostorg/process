/*
 * async_handler.hpp
 *
 *  Created on: 03.04.2016
 *      Author: Klemens
 */

#ifndef BOOST_PROCESS_WINDOWS_ASYNC_HANDLER_HPP_
#define BOOST_PROCESS_WINDOWS_ASYNC_HANDLER_HPP_

#include <boost/process/detail/handler_base.hpp>
#include <type_traits>

namespace boost { namespace process { namespace detail { namespace windows {

struct async_handler : boost::process::detail::handler_base
{
};

template<typename T>
struct is_async_handler :  std::is_base_of<async_handler, T> {};
template<typename T>
struct is_async_handler<T&> :  std::is_base_of<async_handler, T> {};
template<typename T>
struct is_async_handler<const T&> :  std::is_base_of<async_handler, T> {};



}}}}

#endif /* BOOST_PROCESS_WINDOWS_ASYNC_HANDLER_HPP_ */
