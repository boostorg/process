// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_OPTIONS_MAKE_OPTIONS_HPP_
#define BOOST_PROCESS_OPTIONS_MAKE_OPTIONS_HPP_

#include <boost/process/detail/config.hpp>

#if defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/tuple.hpp>
#elif defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/tuple.hpp>
#endif

namespace boost
{
namespace process
{
namespace detail
{

template<typename T>
struct option_t;

template<typename ... Args>
struct options
{
	using needs_io_service = typename needs_io_service<Args...>::type;

	std::conditional<needs_io_service,
		::boost::process::detail::api::initializer_tuple<api::io_service_ref, option_t<Args>...>,
		::boost::process::detail::api::initializer_tuple<option_t<Args>>>;
};

template<typename ... Args>
using options_t = typename options<Args...>::type;

}

template<typename ...Args>
inline detail::options_t<Args...> make_options(const Args & ...) {return {};}

}
}

#endif
