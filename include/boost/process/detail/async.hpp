// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_ASYNC_HPP_
#define BOOST_PROCESS_DETAIL_ASYNC_HPP_

#include <boost/asio/io_service.hpp>


namespace boost { namespace process { namespace detail {

struct async_tag {};
inline async_tag initializer_tag(const boost::asio::io_service&) { return {};}

}}}



#endif /* INCLUDE_BOOST_PROCESS_DETAIL_ASYNC_HPP_ */
