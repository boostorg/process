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
 * \file boost/process/system.hpp
 *
 * Defines a system function.
 */

#ifndef BOOST_PROCESS_SYSTEM_HPP
#define BOOST_PROCESS_SYSTEM_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/on_exit.hpp>
#include <boost/process/child.hpp>
#include <boost/process/detail/async_handler.hpp>
#include <boost/process/detail/execute_impl.hpp>
#include <type_traits>
#include <mutex>
#include <condition_variable>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix.hpp>
#endif

namespace boost {

namespace process {

namespace detail
{

template<typename ...Args>
inline int system_impl(
        std::true_type, /*has async */
        std::true_type, /*has io_service*/
        Args && ...args)
{
    boost::asio::io_service & ios = ::boost::process::detail::get_io_service_var(args...);

    if (ios.stopped())
    {
        child c(std::forward<Args>(args)...);
        if (!c.valid())
            return -1;
        ios.run();
        return c.exit_code();
    }
    else
    {
        std::condition_variable cv;
        std::atomic<bool> exited{false};
        child c(std::forward<Args>(args)...,
                ::boost::process::on_exit(
                [&](int exit_code, const std::error_code&)
                {
                    exited.store(true);
                    cv.notify_all();
                }));

        if (!c.valid())
            return -1;

        std::mutex mtx;
        std::unique_lock<std::mutex> lock(mtx);
        //since this takes a while, I'll check again
        if (ios.stopped())
            ios.run();
        else
            cv.wait(lock, [&]{return exited.load();});

        return c.exit_code();
    }
}

template<typename ...Args>
inline int system_impl(
        std::true_type, /*has async */
        std::false_type, /*has io_service*/
        Args && ...args)
{
    boost::asio::io_service ios;
    child c(ios, std::forward<Args>(args)...);
    if (!c.valid())
        return -1;

    ios.run();
    return c.exit_code();
}


template<typename ...Args>
inline int system_impl(
        std::false_type, /*has async */
        std::true_type, /*has io_service*/
        Args & ...args)
{
    child c(std::forward<Args>(args)...);
    if (!c.valid())
        return -1;
    c.wait();
    return c.exit_code();
}

template<typename ...Args>
inline int system_impl(
        std::false_type, /*has async */
        std::false_type, /*has io_service*/
        Args & ...args)
{
    child c(std::forward<Args>(args)...);
    if (!c.valid())
        return -1;
    c.wait();
    return c.exit_code();
}

}

/** Launches a process and waits for its exit. Similar to std::system. */
template<typename ...Args>
inline int system(Args && ...args)
{
    typedef typename ::boost::process::detail::has_async_handler<Args...>::type
            has_async;
    typedef typename ::boost::process::detail::has_io_service<Args...>::type
            has_ios;

    return ::boost::process::detail::system_impl(
            has_async(), has_ios(),
            std::forward<Args>(args)...
#if defined(BOOST_POSIX_API)
            ,::boost::process::posix::sig.dfl()
#endif
            );
}


}}
#endif

