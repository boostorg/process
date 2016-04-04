// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_ASYNC_HPP_
#define BOOST_PROCESS_ASYNC_HPP_

#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/traits.hpp>
#include <boost/process/windows/async_handler.hpp>
#include <boost/type_index.hpp>
#include <boost/asio.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix/close_in.hpp>
#include <boost/process/posix/close_out.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/windows/on_exit.hpp>
#include <boost/process/windows/io_service_ref.hpp>
#endif


namespace boost { namespace process { namespace detail {

struct async_tag {};

struct on_exit_
{
    api::on_exit_ operator= (const std::function<void(int, const std::error_code&)> & f) const {return f;}
    api::on_exit_ operator()(const std::function<void(int, const std::error_code&)> & f) const {return f;}
};


constexpr static on_exit_ on_exit;


template<typename T>
constexpr inline std::false_type is_io_service(const T & t) {return {};}
constexpr inline std:: true_type is_io_service(const api::io_service_ref & t) {return {};}

template<typename Tuple>
constexpr inline asio::io_service& get_io_service(const Tuple & tup)
{
    auto p = *boost::hana::find_if(tup, [](auto * p){return is_io_service(*p);});
    return p->get();
}

constexpr inline std::true_type is_initializer  (const api::on_exit_&)         {return {};}
constexpr inline std::true_type is_initializer  (const api::io_service_ref&)  {return {};}
constexpr inline async_tag initializer_tag  (asio::io_service & io){return {};}

inline api::io_service_ref make_initializer(async_tag&, boost::hana::tuple<boost::asio::io_service*> & lst)
{
    return *boost::hana::at_c<0>(lst);
}


}

using ::boost::process::detail::on_exit;



}}

#endif /* INCLUDE_BOOST_ASYNC_HPP_ */
