// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_ASYNC_IN_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_ASYNC_IN_HPP

#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#if defined (BOOST_PROCESS_USE_FUTURE)
#include <future>
#endif

#include <iostream>

using namespace std;

namespace boost { namespace process { namespace detail { namespace windows {


template<typename Buffer>
struct async_in_buffer : ::boost::process::detail::windows::async_handler
{
    std::shared_ptr<boost::asio::windows::stream_handle> stream_handle;

    Buffer & buf;

#if defined (BOOST_PROCESS_USE_FUTURE)
    std::shared_ptr<std::promise<void>> promise;
    async_in_val operator<(std::future<void> & fut)
    {
        promise = std::make_shared<std::promise<void>>();
        fut = promise->get_future(); return std::move(*this);
    }
#endif

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(boost::process::pipe::create_async());
    //because the pipe will be moved later on, but i might need the source at another point.
    boost::iostreams::file_descriptor_source source = pipe->source();

    async_in_buffer(Buffer & buf) : buf(buf)
    {
    }
    template <typename Executor>
    inline void on_success(Executor &exec) const
    {
        boost::asio::io_service &is_ser = get_io_service(exec.seq);
        auto stream_handle = this->stream_handle;
        auto pipe = this->pipe;

#if defined (BOOST_PROCESS_USE_FUTURE)
        if (this->promise)
        {
            auto promise = this->promise;

            boost::asio::async_write(*stream_handle, buf,
                [promise](const boost::system::error_code & ec, std::size_t)
                {
                    std::error_code e(ec.value(), std::system_category());
                    promise->set_exception(std::make_exception_ptr(std::system_error(e)));
                    promise->set_value();
                });
        }
        else
#endif
        boost::asio::async_write(*stream_handle, buf,
                [stream_handle, pipe](const boost::system::error_code&ec, std::size_t size){});
    }

    template<typename Executor>
    auto on_exit_handler(Executor & exec)
    {
        auto stream_handle = this->stream_handle;
        auto pipe = this->pipe;
        return [stream_handle, pipe](const std::error_code & ec)
                {
                    boost::asio::io_service & ios = stream_handle->get_io_service();
                    ios.post([stream_handle]
                              {
                                    boost::system::error_code ec;
                                    stream_handle->close(ec);
                              });
                };

    };
    template <typename WindowsExecutor>
    void on_setup(WindowsExecutor &exec)
    {
        stream_handle = std::make_shared<boost::asio::windows::stream_handle>(get_io_service(exec.seq), pipe->sink().handle());

        boost::detail::winapi::SetHandleInformation(source.handle(),
                boost::detail::winapi::HANDLE_FLAG_INHERIT_,
                boost::detail::winapi::HANDLE_FLAG_INHERIT_);

        exec.startup_info.hStdInput = source.handle();
        exec.startup_info.dwFlags  |= boost::detail::winapi::STARTF_USESTDHANDLES_;
        exec.inherit_handles = true;
    }
};


}}}}

#endif
