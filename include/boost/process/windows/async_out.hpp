// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_ASYNC_OUT_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_ASYNC_OUT_HPP

#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/process/windows/pipe.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/asio/windows/stream_handle.hpp>
#include <boost/asio/read.hpp>
#include <istream>
#include <memory>
#include <exception>

#if defined (BOOST_PROCESS_USE_FUTURE)
#include <future>
#endif

namespace boost { namespace process { namespace detail { namespace windows {


template <typename Executor>
void apply_out_handles(Executor &e, void* handle, std::integral_constant<int, 1>, std::integral_constant<int, -1>)
{
    boost::detail::winapi::SetHandleInformation(handle,
            boost::detail::winapi::HANDLE_FLAG_INHERIT_,
            boost::detail::winapi::HANDLE_FLAG_INHERIT_);

    e.startup_info.hStdOutput = handle;
    e.startup_info.dwFlags   |= ::boost::detail::winapi::STARTF_USESTDHANDLES_;
    e.inherit_handles = true;
}

template <typename Executor>
void apply_out_handles(Executor &e, void* handle, std::integral_constant<int, 2>, std::integral_constant<int, -1>)
{
    boost::detail::winapi::SetHandleInformation(handle,
            boost::detail::winapi::HANDLE_FLAG_INHERIT_,
            boost::detail::winapi::HANDLE_FLAG_INHERIT_);


    e.startup_info.hStdError = handle;
    e.startup_info.dwFlags  |= ::boost::detail::winapi::STARTF_USESTDHANDLES_;
    e.inherit_handles = true;
}

template <typename Executor>
void apply_out_handles(Executor &e, void* handle, std::integral_constant<int, 1>, std::integral_constant<int, 2>)
{
    boost::detail::winapi::SetHandleInformation(handle,
            boost::detail::winapi::HANDLE_FLAG_INHERIT_,
            boost::detail::winapi::HANDLE_FLAG_INHERIT_);

    e.startup_info.hStdOutput = handle;
    e.startup_info.hStdError  = handle;
    e.startup_info.dwFlags   |= ::boost::detail::winapi::STARTF_USESTDHANDLES_;
    e.inherit_handles = true;
}



template<int p1, int p2, typename Buffer>
struct async_out_buffer : ::boost::process::detail::windows::async_handler
{
    std::shared_ptr<boost::asio::windows::stream_handle> stream_handle;

    Buffer & buf;

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(boost::process::pipe::create_async());
    //because the pipe will be moved later on, but i might need the source at another point.
    boost::iostreams::file_descriptor_sink sink = pipe->sink();

    async_out_buffer(Buffer & buf) : buf(buf)
    {
    }
    template <typename Executor>
    inline void on_success(Executor &exec) const
    {
        boost::asio::io_service &is_ser = get_io_service(exec.seq);
        auto& stream_handle = this->stream_handle;
        auto pipe = this->pipe;
        boost::asio::async_read(*stream_handle, buf,
                [stream_handle, pipe](const boost::system::error_code&, std::size_t size){});
    }

    template<typename Executor>
    auto on_exit_handler(Executor & exec)
    {
        auto  stream_handle = this->stream_handle;
        auto &pipe = this->pipe;
        return [stream_handle, pipe](const std::error_code & ec)
                {
                    boost::asio::io_service & ios = stream_handle->get_io_service();
                    ios.post([stream_handle]{stream_handle->close();});

                };

    };
    template <typename WindowsExecutor>
    void on_setup(WindowsExecutor &exec)
    {
        stream_handle = std::make_shared<boost::asio::windows::stream_handle>(get_io_service(exec.seq), pipe->source().handle());
        apply_out_handles(exec, sink.handle(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
    }
};

template<typename T> struct deduce_async_out_cb_type;
template<typename T> struct deduce_async_out_cb_type<const std::function<void(T)>> {using type = T;};


struct completion_handler {};

template<int p1, int p2, typename Callback>
struct async_out_cb : ::boost::process::detail::windows::async_handler
{
    Callback cb;

    using argument_type = typename deduce_async_out_cb_type<Callback>::type;

    std::shared_ptr<boost::asio::windows::stream_handle> stream_handle;
    std::shared_ptr<boost::asio::streambuf> buffer = std::make_shared<boost::asio::streambuf>();

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(boost::process::pipe::create_async());
    //because the pipe will be moved later on, but i might need the source at another point.
    boost::iostreams::file_descriptor_sink sink = pipe->sink();

    async_out_cb(const Callback & cb) : cb(cb) {}

    template <typename Executor>
    inline void on_success(Executor &exec) const
    {
        boost::asio::io_service &is_ser = get_io_service(exec.seq);
        auto& stream_handle = this->stream_handle;
        auto pipe   = this->pipe;
        auto buffer = this->buffer;
        auto cb     = this->cb;
        auto func_p = std::make_shared<std::function<std::size_t(const boost::system::error_code&, std::size_t)>>();

        *func_p = [stream_handle, pipe, buffer, cb,  func_p](const boost::system::error_code&, std::size_t size)
                  {
                      if (buffer->size() > 0)
                      {
                          std::istream is (buffer.get());
                          argument_type arg;
                          arg.resize(buffer->size());
                          is.read(&*arg.begin(), buffer->size());
                          cb(std::move(arg));
                      }
                      if (stream_handle->is_open())
                      {
                          return 1024u;
                      }
                      else
                          return 0u;
                  };

        boost::asio::async_read(*stream_handle, *buffer, *func_p, [](const boost::system::error_code & , std::size_t){});
    }

    template<typename Executor>
    auto on_exit_handler(Executor & exec)
    {
        auto  stream_handle = this->stream_handle;
        auto & pipe = this->pipe;
        return [stream_handle, pipe](const std::error_code & ec)
                {
                    boost::asio::io_service & ios = stream_handle->get_io_service();
                    ios.post([stream_handle]{stream_handle->close();});

                };

    };
    template <typename WindowsExecutor>
    void on_setup(WindowsExecutor &exec)
    {
        stream_handle = std::make_shared<boost::asio::windows::stream_handle>(get_io_service(exec.seq), pipe->source().handle());
        apply_out_handles(exec, sink.handle(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
    }
};

#if defined (BOOST_PROCESS_USE_FUTURE)

template<int p1, int p2, typename Type>
struct async_out_future : ::boost::process::detail::windows::async_handler
{
    std::shared_ptr<std::promise<Type>> promise = std::shared_ptr<std::promise<Type>>;

    std::shared_ptr<boost::asio::windows::stream_handle> stream_handle;
    std::shared_ptr<boost::asio::streambuf> buffer = std::make_shared<boost::asio::streambuf>();

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(boost::process::pipe::create_async());
    //because the pipe will be moved later on, but i might need the source at another point.
    boost::iostreams::file_descriptor_sink sink = pipe->sink();

    async_out_future(std::future<Type> & fut)
    {
        fut = promise->get_future();
    }
    template <typename Executor>
    inline void on_success(Executor &exec) const
    {
        boost::asio::io_service &is_ser = get_io_service(exec.seq);
        auto& stream_handle = this->stream_handle;
        auto pipe    = this->pipe;
        auto buffer  = this->buffer;
        auto promise = this->promise;
        boost::asio::async_read(*stream_handle, *buffer,
                [stream_handle, pipe, buffer, promise](const boost::system::error_code& ec, std::size_t size)
                {
                    if (ec)
                    {
                        std::error_code e(ec.value(), std::system_category());
                        promise->set_exception(std::make_exception_ptr(std::system_error(e)));
                    }
                    else
                    {
                        std::istream is (buffer.get());
                        Type arg;
                        arg.resize(buffer->size());
                        is.read(&*arg.begin(), buffer->size());

                        promise->set_value(std::move(arg));
                    }
                });
    }

    template<typename Executor>
    auto on_exit_handler(Executor & exec)
    {
        auto  stream_handle = this->stream_handle;
        auto &pipe = this->pipe;

        return [stream_handle, pipe](const std::error_code & ec)
                {
                    boost::asio::io_service & ios = stream_handle->get_io_service();
                    ios.post([stream_handle]{stream_handle->close();});

                };

    };
    template <typename WindowsExecutor>
    void on_setup(WindowsExecutor &exec)
    {
        stream_handle = std::make_shared<boost::asio::windows::stream_handle>(get_io_service(exec.seq), pipe->source().handle());
        apply_out_handles(exec, sink.handle(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
    }
};
#endif


}}}}

#endif
