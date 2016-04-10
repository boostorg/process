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
constexpr auto is_async_handler(const T& t) -> std::is_base_of<async_handler, T> {return {};}


}}}}

#endif /* BOOST_PROCESS_WINDOWS_ASYNC_HANDLER_HPP_ */
