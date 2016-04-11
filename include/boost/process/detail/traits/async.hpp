// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_TRAITS_DECL_HPP_
#define BOOST_PROCESS_DETAIL_TRAITS_DECL_HPP_

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/traits/decl.hpp>

namespace boost { namespace asio {

struct io_service;
}}

namespace boost { namespace process { namespace detail {

struct async_tag {};

template<>
struct make_initializer_t<async_tag>;

template<> struct initializer_tag_t<boost::asio::io_service> { typedef async_tag type;};




}}}



#endif /* BOOST_PROCESS_DETAIL_HANDLER_HPP_ */
