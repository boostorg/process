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

template<typename IoService, typename ...Args>
inline int system_impl(
        std::true_type, /*has async */
        std::true_type, /*has io_service*/
        std::false_type, /* has_yield */
        Args && ...args)
{
    IoService & ios = ::boost::process::detail::get_io_service_var(args...);


    std::atomic_bool exited{false};

    child c(std::forward<Args>(args)...,
            ::boost::process::on_exit(
                [&](int exit_code, const std::error_code&)
                {
                    ios.post([&]{exited.store(true);});
                }));
    if (!c.valid())
        return -1;

    while (!exited.load())
        ios.poll();

    return c.exit_code();
}

template<typename IoService, typename ...Args>
inline int system_impl(
        std::true_type, /*has async */
        std::false_type, /*has io_service*/
        std::false_type, /* has_yield */
        Args && ...args)
{
    IoService ios;
    child c(ios, std::forward<Args>(args)...);
    if (!c.valid())
        return -1;

    ios.run();
    return c.exit_code();
}


template<typename IoService, typename ...Args>
inline int system_impl(
        std::false_type, /*has async */
        std::true_type, /*has io_service*/
        std::false_type, /* has_yield */
        Args && ...args)
{
    child c(std::forward<Args>(args)...);
    if (!c.valid())
        return -1;
    c.wait();
    return c.exit_code();
}

template<typename IoService, typename ...Args>
inline int system_impl(
        std::false_type, /*has async */
        std::false_type, /*has io_service*/
        std::false_type, /* has_yield */
        Args && ...args)
{
    child c(std::forward<Args>(args)...
#if defined(BOOST_POSIX_API)
            ,::boost::process::posix::sig.dfl()
#endif
			);
    if (!c.valid())
        return -1;
    c.wait();
    return c.exit_code();
}

template<typename T>
constexpr T& remove_yield(T& t) noexcept
{
    return t;
}

template<typename T>
constexpr T&& remove_yield(T&& t) noexcept
{
    return static_cast<T&&>(t);
}

template<typename Handler>
constexpr ::boost::process::detail::handler
    remove_yield(::boost::asio::basic_yield_context<Handler> & yield_) noexcept
{
    return {}; //essentially nop.
}

template<typename Handler>
constexpr ::boost::process::detail::handler
    remove_yield(::boost::asio::basic_yield_context<Handler> && yield_) noexcept
{
    return {}; //essentially nop.
}

template<typename ...Args>
struct get_yield_t;

template<typename First, typename ... Args>
struct get_yield_t<First, Args...>
{
    typedef typename get_yield_t<Args...>::type type;
};

template<typename First, typename ... Args>
struct get_yield_t<boost::asio::basic_yield_context<First>, Args...>
{
    typedef boost::asio::basic_yield_context<First> type;
};

template<typename First>
struct get_yield_t<boost::asio::basic_yield_context<First>>
{
    typedef boost::asio::basic_yield_context<First> type;
};

template<typename Handler, typename ...Args>
auto get_yield(boost::asio::basic_yield_context<Handler> &&yield_) ->
        boost::asio::basic_yield_context<Handler> &
{
    return yield_;
}

template<typename Handler, typename ...Args>
auto get_yield(boost::asio::basic_yield_context<Handler> &yield_) ->
        boost::asio::basic_yield_context<Handler> &
{
    return yield_;
}


template<typename First, typename ...Args>
auto get_yield(First &&f, Args&&...args) ->
    typename get_yield_t<typename std::remove_reference<Args>::type...>::type &
{
    return get_yield(args...);
}

template<typename Handler, typename ...Args>
auto get_yield(boost::asio::basic_yield_context<Handler> &&yield_, Args&&...args) ->
    typename get_yield_t<typename std::remove_reference<Args>::type...>::type &
{
    return yield_;
}

template<typename Handler, typename ...Args>
auto get_yield(boost::asio::basic_yield_context<Handler> &yield_, Args&&...args) ->
    typename get_yield_t<typename std::remove_reference<Args>::type...>::type &
{
    return yield_;
}


template<typename IoService, typename T1, typename T2, typename ...Args>
inline int system_impl(
        T1, /*has async */
        T2, /*has io_service*/
        std::true_type, /* has_yield */
        Args && ...args)
{

    auto &yield_ = get_yield(args...);

    auto coro = yield_.coro_.lock();
    auto & ios = yield_.handler_.dispatcher_.get_io_service();
    BOOST_ASSERT(coro.use_count());

    int ret = -1;
    auto l = [coro, &ret, &ios](int ret_, const std::error_code& ec_)
    {
        ret = ret_;
        ios.post(
                [coro]
                {
                    auto & c = *coro;
                    if (c)
                        c();
                 });
    };

    child c(remove_yield(std::forward<Args>(args))...,
            ios,
            boost::process::on_exit=l);
    if (!c.valid())
        return -1;

    yield_.ca_();

    return ret;
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
    typedef typename ::boost::process::detail::has_yield_context<Args...>::type
            has_yield;

    return ::boost::process::detail::system_impl<boost::asio::io_service>(
            has_async(), has_ios(), has_yield(),
            std::forward<Args>(args)...);
}


}}
#endif

