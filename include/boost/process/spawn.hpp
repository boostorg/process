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
 * \file boost/process/spawn.hpp
 *
 * Defines the spawn function.
 */

#ifndef BOOST_PROCESS_SPAWN_HPP
#define BOOST_PROCESS_SPAWN_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/child_decl.hpp>
#include <boost/process/detail/execute_impl.hpp>
#include <boost/process/detail/async_handler.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix.hpp>
#endif

namespace boost {

namespace process {

namespace detail {

}

/** Launch a process and detach it. Returns no handle.
 *
 */
template<typename ...Args>
inline void spawn(Args && ...args)
{
    typedef typename ::boost::process::detail::has_async_handler<Args...>::type
            has_async;
    typedef typename ::boost::process::detail::has_io_service<Args...>::type
            has_ios;

    static_assert(
            (!has_async::value) && (!has_ios::value),
            "Spawn cannot wait for exit, so implicit asyncs "
            "and asio::io_service cannot be passed");

    auto c = ::boost::process::detail::execute_impl(
#if defined(BOOST_POSIX_API)
            ::boost::process::posix::sig.ign(),
#endif
             std::forward<Args>(args)...);
    c.detach();
}

}}
#endif

