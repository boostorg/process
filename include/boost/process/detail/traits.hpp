// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_TRAITS_HPP_
#define BOOST_PROCESS_DETAIL_TRAITS_HPP_

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
std::is_base_of<handler_base, T> is_initializer(const T &){return {};}

template<typename T>
boost::none_t initializer_tag(const boost::none_t&);// {return boost::none;}
template<typename ...Args>
void make_initializer(boost::none_t, Args&&...);

//template<typename ...Args>
//int make_initializer(Args&&...);
//

template<typename T>
boost::none_t initializer_resource(const T & ) {return boost::none;}


}}}



#endif /* BOOST_PROCESS_DETAIL_HANDLER_HPP_ */
