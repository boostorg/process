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
 * \file boost/process/async_system.hpp
 *
 * Defines the asynchrounous version of the system function.
 */

#ifndef BOOST_PROCESS_ASYNC_SYSTEM_HPP
#define BOOST_PROCESS_ASYNC_SYSTEM_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/async.hpp>
#include <boost/process/child.hpp>
#include <boost/process/detail/async_handler.hpp>
#include <boost/process/detail/execute_impl.hpp>
#include <type_traits>
#include <memory>
#include <boost/asio/async_result.hpp>
#include <boost/system/error_code.hpp>
#include <tuple>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix.hpp>
#endif

namespace boost {
namespace process {
namespace detail
{

template<typename ExitHandler>
struct async_system_handler : detail::api::async_handler
{
    boost::asio::io_service & ios;
    boost::asio::detail::async_result_init<
            ExitHandler, void(boost::system::error_code, int)> init;

    template<typename ExitHandler_>
    async_system_handler(
    		boost::asio::io_service & ios,
			ExitHandler_ && exit_handler) : ios(ios), init(std::forward<ExitHandler_>(exit_handler))
    {

    }


    template<typename Exec>
    void on_error(Exec & exec, const std::error_code & ec)
    {
        auto & h = init.handler;
        ios.post(
                [h, ec]() mutable
                {
                    h(boost::system::error_code(ec.value(), boost::system::system_category()), -1);
                });
    }

    BOOST_ASIO_INITFN_RESULT_TYPE(ExitHandler, void (boost::system::error_code, int))
        get_result()
    {
        return init.result.get();
    }

    template<typename Executor>
    std::function<void(int, const std::error_code&)> on_exit_handler(Executor & exec)
    {
        auto & h = init.handler;
        return [h](int exit_code, const std::error_code & ec) mutable
               {
                    h(boost::system::error_code(ec.value(), boost::system::system_category()), exit_code);
               };
    }
};


template<typename ExitHandler>
struct is_error_handler<async_system_handler<ExitHandler>>    : std::true_type {};

}

template<typename ExitHandler, typename ...Args>
inline BOOST_ASIO_INITFN_RESULT_TYPE(ExitHandler, void (boost::system::error_code, int))
    async_system(boost::asio::io_service & ios, ExitHandler && exit_handler, Args && ...args)
{
    detail::async_system_handler<ExitHandler> async_h{ios, std::forward<ExitHandler>(exit_handler)};

    typedef typename ::boost::process::detail::has_error_handler<boost::fusion::tuple<Args...>>::type
            has_err_handling;

    static_assert(!has_err_handling::value, "async_system cannot have custom error handling");


    child(ios, std::forward<Args>(args)..., async_h ).detach();

    return async_h.get_result();
}




}}
#endif

