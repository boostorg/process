// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/child.hpp
 *
 * Defines a child process class.
 */

#ifndef BOOST_PROCESS_CHILD_HPP
#define BOOST_PROCESS_CHILD_HPP

#include <boost/process/detail/child_decl.hpp>
#include <boost/process/detail/execute_impl.hpp>

namespace boost {

namespace process {

template<typename ...Args>
child::child(Args&&...args) : child(detail::execute_impl(std::forward<Args>(args)...)) {}


template<typename ...Args>
inline void spawn(Args && ...args)
{
	child c(std::forward<Args>(args)...);
	c.detach();
}


template<typename ...Args>
inline int system(Args && ...args)
{
	child c(std::forward<Args>(args)...);
	c.wait();
	return c.exit_code();
}



}}
#endif

