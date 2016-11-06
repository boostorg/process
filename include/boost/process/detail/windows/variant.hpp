// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_VARIANT_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_VARIANT_HPP_

#include <boost/process/detail/windows/handler.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/process/detail/async_handler.hpp>

namespace boost
{
namespace process
{
namespace detail
{
namespace windows
{


template<typename Executor>
struct on_setup_visitor : boost::static_visitor<void>
{
	Executor & exec;

	template<typename T>
	void operator()(T &t)
	{
		t.on_setup(exec);
	}
};

template<typename Executor>
struct on_error_visitor : boost::static_visitor<void>
{
	Executor & exec;
	const std::error_code &ec;

	template<typename T>
	void operator()(T &t)
	{
		t.on_error(exec, ec);
	}
};

template<typename Executor>
struct on_success_visitor : boost::static_visitor<void>
{
	Executor & exec;

	template<typename T>
	void operator()(T &t)
	{
		t.on_success(exec);
	}
};


struct variant_require_ios_tag_empty {};

template<typename ...Args>
struct variant_require_ios_tag
{
	using needs_io_service = typename needs_io_service<Args...>::type;
	using type = typename std::conditional<
			needs_io_service,
			require_io_service,
			variant_require_ios_tag_empty>::type;
};


template<typename ...Args>
using variant_require_ios_tag_t = typename variant_require_ios_tag<Args...>::type;

template<typename ...Args>
struct handler_variant : handler_base_ext, boost::variant<Args...>, variant_require_ios_tag_t<Args...>
{
	using boost::variant<Args...>::variant;
	using boost::variant<Args...>::operator=;

    template <class Executor>
    void on_setup(Executor& exec)
    {
    	on_setup_visitor<Executor> vis{exec};
    	boost::apply_visitor(vis, *this);
    }

    template <class Executor>
    void on_error(Executor& exec, const std::error_code & ec)
    {
    	on_error_visitor<Executor> vis{exec, ec};
    	boost::apply_visitor(vis, *this);
    }

    template <class Executor>
    void on_success(Executor& exec)
    {
    	on_success_visitor<Executor> vis{exec};
    	boost::apply_visitor(vis, *this);
    }
};


}
}
}
}



#endif
