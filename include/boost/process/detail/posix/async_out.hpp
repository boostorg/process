// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_ASYNC_OUT_HPP
#define BOOST_PROCESS_DETAIL_POSIX_ASYNC_OUT_HPP


#include <boost/process/detail/posix/pipe.hpp>
#include <boost/process/detail/posix/handler.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read.hpp>
#include <istream>
#include <memory>
#include <exception>

#if defined (BOOST_PROCESS_USE_FUTURE)
#include <future>
#endif

namespace boost { namespace process { namespace detail { namespace posix {


inline void apply_out_handles(int handle, std::integral_constant<int, 1>, std::integral_constant<int, -1>)
{
    ::dup2(handle, STDOUT_FILENO);
}

inline void apply_out_handles(int handle, std::integral_constant<int, 2>, std::integral_constant<int, -1>)
{
    ::dup2(handle, STDERR_FILENO);
}

inline void apply_out_handles(int handle, std::integral_constant<int, 1>, std::integral_constant<int, 2>)
{
    ::dup2(handle, STDOUT_FILENO);
    ::dup2(handle, STDERR_FILENO);
}

template<int p1, int p2, typename Buffer>
struct async_out_buffer : ::boost::process::detail::posix::async_handler
{
    std::shared_ptr<boost::asio::posix::stream_descriptor> stream_descriptor;

    Buffer & buf;

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(pipe::create_async());
    //because the pipe will be moved later on, but i might need the source at another point.
    boost::iostreams::file_descriptor_sink sink = pipe->sink();

    async_out_buffer(Buffer & buf) : buf(buf)
    {
    }

    template <typename Executor>
    inline void on_success(Executor &exec) const
    {
        pipe->sink().close();
        boost::asio::io_service &is_ser = get_io_service(exec.seq);
        auto& stream_descriptor = this->stream_descriptor;
        auto  pipe              = this->pipe;
        boost::asio::async_read(*stream_descriptor, buf,
                [stream_descriptor, pipe](const boost::system::error_code&, std::size_t size){});
    }

    template<typename Executor>
    std::function<void(const std::error_code&)> on_exit_handler(Executor & exec)
    {

        this->stream_descriptor = std::make_shared<boost::asio::posix::stream_descriptor>(get_io_service(exec.seq), pipe->source().handle());

        auto stream_descriptor = this->stream_descriptor;
        auto pipe = this->pipe;
        return [stream_descriptor, pipe](const std::error_code & ec)
                {
                    boost::asio::io_service & ios = stream_descriptor->get_io_service();
                    ios.post([stream_descriptor]
                              {
                                    boost::system::error_code ec;
                                    stream_descriptor->close(ec);
                              });
                };

    };

    template <typename Executor>
    void on_exec_setup(Executor &exec)
    {
        apply_out_handles(sink.handle(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());

        pipe->source().close();
    }
};



#if defined (BOOST_PROCESS_USE_FUTURE)

template<int p1, int p2, typename Type>
struct async_out_future : ::boost::process::detail::posix::async_handler
{
    std::shared_ptr<std::promise<Type>> promise = std::make_shared<std::promise<Type>>();

    std::shared_ptr<boost::asio::posix::stream_descriptor> stream_descriptor;
    std::shared_ptr<boost::asio::streambuf> buffer = std::make_shared<boost::asio::streambuf>();

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(boost::process::detail::api::pipe::create_async());
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
        auto& stream_descriptor = this->stream_descriptor;
        auto pipe    = this->pipe;
        auto buffer  = this->buffer;
        auto promise = this->promise;
        boost::asio::async_read(*stream_descriptor, *buffer,
                [stream_descriptor, pipe, buffer, promise](const boost::system::error_code& ec, std::size_t size)
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
    std::function<void(const std::error_code&)> on_exit_handler(Executor & exec)
    {

        stream_descriptor = std::make_shared<boost::asio::posix::stream_descriptor>(get_io_service(exec.seq), pipe->source().handle());

        auto stream_descriptor = this->stream_descriptor;
        auto pipe = this->pipe;
        return [stream_descriptor, pipe](const std::error_code & ec)
                {
                    boost::asio::io_service & ios = stream_descriptor->get_io_service();
                    ios.post([stream_descriptor]
                              {
                                    boost::system::error_code ec;
                                    stream_descriptor->close(ec);
                              });
                };


    };

    template <typename Executor>
    void on_exec_setup(Executor &exec)
    {
        apply_out_handles(exec, sink.handle(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
    }

};

#endif

}}}}

#endif
