// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_ON_EXIT_HPP_
#define BOOST_PROCESS_WINDOWS_ON_EXIT_HPP_

#include <boost/process/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/process/windows/on_exit.hpp>
#include <boost/process/windows/async_handler.hpp>
#include <boost/detail/winapi/process.hpp>
#include <system_error>
#include <functional>
#include <iostream>

namespace boost { namespace process { namespace detail { namespace windows {

struct on_exit_: boost::process::detail::windows::async_handler
{
    std::function<void(int, const std::error_code&)> handler;
    on_exit_(const std::function<void(int, const std::error_code&)> & handler) : handler(handler)
    {

    }

    template<typename Executor>
    auto on_exit_handler(Executor & exec) const
    {
        auto handler = this->handler;
        ::boost::detail::winapi::PROCESS_INFORMATION_ & proc = exec.proc_info;
        auto process_handle = proc.hProcess;

        return [handler, process_handle](const std::error_code & ec_in)
                {
            std::cout << "on_exit_handler" << std::endl;
                    ::boost::detail::winapi::DWORD_ exit_code = 0;
                    auto ec = ec_in;
                    if (!ec)
                    {
                        auto err_code = ::boost::detail::winapi::GetExitCodeProcess(process_handle, &exit_code);
                        if (err_code)
                            ec = boost::process::detail::get_last_error();
                    }
                    handler(static_cast<int>(exit_code), ec);
                };

    };
};


}}}}
#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ON_EXIT_HPP_ */
