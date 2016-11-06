// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_IO_VARIANT_HPP_
#define BOOST_PROCESS_IO_VARIANT_HPP_

#include <boost/process/variant.hpp>
#include <boost/process/io.hpp>

namespace boost
{
namespace process
{

namespace detail
{

using std_in_variant_base =
		handler_variant<
		    api::null_in,
		    api::close_in,
			api::pipe_in,
			api::file_in,
			api::async_in_buffer<boost::asio::mutable_buffer>,
			api::async_in_buffer<boost::asio::const_buffer>,
			api::async_in_buffer<boost::asio::mutable_buffers_1>,
			api::async_in_buffer<boost::asio::const_buffers_1>,
			api::async_in_buffer<boost::asio::streambuf>>;

template<int p1, int p2>
using std_sink_variant_base =
		handler_variant<
		    api::null_out<p1, p2>,
			api::close_out<p1, p2>,
			api::pipe_out<p1, p2>,
			api::file_out<p1, p2>,
		    api::async_out_buffer<p1, p2, const asio::mutable_buffer> ,
		    api::async_out_buffer<p1, p2, const asio::mutable_buffers_1>,
		    api::async_out_buffer<p1, p2, asio::streambuf>,
		    api::async_out_buffer<p1, p2, const asio::mutable_buffer>,
		    api::async_out_buffer<p1, p2, const asio::mutable_buffers_1>,
		    api::async_out_buffer<p1, p2, asio::streambuf>,
		    api::async_out_future<p1, p2, std::string>,
		    api::async_out_future<p1, p2, std::string>,
		    api::async_out_future<p1, p2, std::vector<char>>,
		    api::async_out_future<p1, p2, std::vector<char>>>;

template<int p1, int p2 = -1>
struct std_sink_variant : std_sink_variant_base<p1, p2>,
                          ::boost::process::detail::api::require_io_service
{
	using std_sink_variant_base<p1, p2>::std_sink_variant_base;
	using std_sink_variant_base<p1, p2>::operator=;

	void null()  {*this = api:: null_out<p1, p2>();}
	void close() {*this = api::close_out<p1, p2>();}

	template<typename T>
	void set(T& t)
	{
		*this = (std_out_<p1, p2>() > t);
	}
};

}


struct std_in_variant : detail::std_in_variant_base,
                        ::boost::process::detail::api::require_io_service
{
	using detail::std_in_variant_base::std_in_variant_base;
	using detail::std_in_variant_base::operator=;

	void null()  {*this = detail::api:: null_in();}
	void close() {*this = detail::api::close_in();}

	template<typename T>
	void set(T& t)
	{
		*this = (std_in < t);
	}
};

using std_out_variant     = ::boost::process::detail::std_sink_variant_base<1>;
using std_err_variant     = ::boost::process::detail::std_sink_variant_base<2>;
using std_out_err_variant = ::boost::process::detail::std_sink_variant_base<1,2>;

}
}

#endif
