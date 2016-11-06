// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/** \file boost/process/options/async.hpp

The header which provides the asynchrounous features for the option style.
  */

#ifndef BOOST_PROCESS_ASYNC_HPP_
#define BOOST_PROCESS_ASYNC_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/process/async.hpp>
#include <boost/process/options/make_options.hpp>

namespace boost
{
namespace process
{
namespace detail
{

template<>
class options_t<api::on_exit_> : api::async_handler
{
    std::vector<std::function<void(int, const std::error_code&)>> _handlers;
public:

    template<typename Executor>
    std::function<void(int, const std::error_code&)> on_exit_handler(Executor & exec)
    {
        return [_handlers](int exit_code, const std::error_code & ec)
               {
        			for (auto & h : _handlers)
        				h(static_cast<int>(exit_code), ec);
               };
    }

    void add_on_exit(const std::function<void(int, const std::error_code&)> &f)
    {
    	_handlers.push_back(f);
    }
    void add_on_exit(std::future<int> &f)
	{
		_handlers.push_back(::boost::process::detail::on_exit_from_future(f));
	}
};

template<>
class options_t<api::io_service_ref> : api::io_service_ref
{
	boost::asio::io_service _ios;
public:
	options_t() : api::io_service_ref(_ios) {}

	boost::asio::io_service & get_io_service() {return _ios;}
};



}
}
}



#endif /* INCLUDE_BOOST_PROCESS_DETAIL_ASYNC_HPP_ */
