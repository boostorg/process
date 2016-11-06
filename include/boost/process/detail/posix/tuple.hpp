// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_TUPLE_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_TUPLE_HPP_

#include <boost/process/detail/windows/handler.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

namespace boost
{
namespace process
{
namespace detail
{
namespace windows
{

template<typename Executor>
struct tuple_on_setup
{
	Executor & exec;
	template<typename T>
	void operator()(T *p)
	{
		p->on_setup(exec);
	}
};

template<typename Executor>
struct tuple_on_error
{
	Executor & exec;
	const std::error_code & ec;
	template<typename T>
	void operator()(T *p)
	{
		p->on_setup(exec);
	}
};

template<typename Executor>
struct tuple_on_success
{
	Executor & exec;
	template<typename T>
	void operator()(T *p)
	{
		p->on_setup(exec);
	}
};


template<typename Executor>
struct tuple_on_fork_error
{
	Executor & exec;
	const std::error_code &ec;

	template<typename T>
	void operator()(T *t)
	{
		t->on_fork_error(exec, ec);
	}
};

template<typename Executor>
struct tuple_on_exec_setup
{
	Executor & exec;

	template<typename T>
	void operator()(T *t)
	{
		t->on_exec_setup(exec);
	}
};


template<typename Executor>
struct tuple_on_exec_error
{
	Executor & exec;
	const std::error_code &ec;

	template<typename T>
	void operator()(T *t)
	{
		t->on_exec_error(exec, ec);
	}
};

template<typename ...Args>
struct initializer_tuple : Args...
{
    template <class Executor>
    void on_setup(Executor& exec)
    {
    	auto tup = boost::fusion::make_tuple(static_cast<Args*>(this)...);
    	tuple_on_setup<Executor> func{exec};
    	boost::fusion::for_each(tup, func);
    }

    template <class Executor>
    void on_error(Executor& exec, const std::error_code & ec)
    {
    	auto tup = boost::fusion::make_tuple(static_cast<Args*>(this)...);
    	tuple_on_setup<Executor> func{exec};
    	boost::fusion::for_each(tup, func);
    }

    template <class Executor>
    void on_success(Executor& exec)
    {
    	auto tup = boost::fusion::make_tuple(static_cast<Args*>(this)...);
    	tuple_on_setup<Executor> func{exec};
    	boost::fusion::for_each(tup, func);
    }

    template<typename Executor>
    void on_fork_error  (Executor & exec, const std::error_code& ec)
    {
    	auto tup = boost::fusion::make_tuple(static_cast<Args*>(this)...);
    	tuple_on_fork_error<Executor> func{exec, ec};
    	boost::fusion::for_each(tup, func);
    }

    template<typename Executor>
    void on_exec_setup  (Executor & exec)
    {
    	auto tup = boost::fusion::make_tuple(static_cast<Args*>(this)...);
    	tuple_on_exec_setup<Executor> func{exec};
    	boost::fusion::for_each(tup, func);
    }

    template<typename Executor>
    void on_exec_error  (Executor & exec, const std::error_code& ec)
    {
    	auto tup = boost::fusion::make_tuple(static_cast<Args*>(this)...);
    	tuple_on_exec_error<Executor> func{exec, ec};
    	boost::fusion::for_each(tup, func);
    }
};

}
}
}
}



#endif
