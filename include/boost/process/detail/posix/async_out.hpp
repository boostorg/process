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


#include <boost/process/detail/posix/handler.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read.hpp>
#include <boost/process/async_pipe.hpp>
#include <istream>
#include <memory>
#include <exception>

#include <future>

namespace boost { namespace process { namespace detail { namespace posix {


inline int apply_out_handles(int handle, std::integral_constant<int, 1>, std::integral_constant<int, -1>)
{
    return ::dup2(handle, STDOUT_FILENO);
}

inline int apply_out_handles(int handle, std::integral_constant<int, 2>, std::integral_constant<int, -1>)
{
    return ::dup2(handle, STDERR_FILENO);
}

inline int apply_out_handles(int handle, std::integral_constant<int, 1>, std::integral_constant<int, 2>)
{
    if (::dup2(handle, STDOUT_FILENO) == -1)
        return -1;
    if (::dup2(handle, STDERR_FILENO) == -1)
        return -1;

    return 0;
}

template<int p1, int p2, typename Buffer>
struct async_out_buffer : ::boost::process::detail::posix::async_handler
{
    Buffer & buf;

    std::shared_ptr<boost::process::async_pipe> pipe;


    async_out_buffer(Buffer & buf) : buf(buf)
    {
    }

    template <typename Executor>
    inline void on_success(Executor &exec)
    {
        auto  pipe              = this->pipe;
        boost::asio::async_read(*pipe, buf,
                [pipe](const boost::system::error_code&, std::size_t size){});

        this->pipe = nullptr;
    }

    template<typename Executor>
    std::function<void(int, const std::error_code&)> on_exit_handler(Executor & exec)
    {
        
        pipe = std::make_shared<boost::process::async_pipe>(get_io_service(exec.seq));
 
        auto pipe = this->pipe;
        return [pipe](int, const std::error_code & ec)
                {
                    boost::asio::io_service & ios = pipe->get_io_service();
                    ios.post([pipe]
                              {
                                    boost::system::error_code ec;
                                    pipe->close(ec);
                              });
                };
    };

    template <typename Executor>
    void on_exec_setup(Executor &exec)
    {
        if (!pipe)
            pipe = std::make_shared<boost::process::async_pipe>(get_io_service(exec.seq));

        int res = apply_out_handles(pipe->native_sink(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
        if (res == -1)
            exec.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
    }
};




template<int p1, int p2, typename Type>
struct async_out_future : ::boost::process::detail::posix::async_handler
{
    std::shared_ptr<std::promise<Type>> promise = std::make_shared<std::promise<Type>>();

    std::shared_ptr<boost::asio::streambuf> buffer = std::make_shared<boost::asio::streambuf>();

    std::shared_ptr<boost::process::async_pipe> pipe;

    async_out_future(std::future<Type> & fut)
    {
        fut = promise->get_future();
    }
    template <typename Executor>
    inline void on_success(Executor &exec)
    {
        if (!pipe)
            pipe = std::make_shared<boost::process::async_pipe>(get_io_service(exec.seq));

        auto pipe = this->pipe;
        auto buffer  = this->buffer;
        auto promise = this->promise;
        boost::asio::async_read(*pipe, *buffer,
                [pipe, buffer, promise](const boost::system::error_code& ec, std::size_t size)
                {
                    if (ec && (ec.value() != EBADF) && (ec.value() == EPERM))
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

        this->pipe = nullptr;

    }

    template<typename Executor>
    std::function<void(int, const std::error_code&)> on_exit_handler(Executor & exec)
    {
        if (!pipe)
            pipe = std::make_shared<boost::process::async_pipe>(get_io_service(exec.seq));

        auto pipe = this->pipe;
        return [pipe](int, const std::error_code & ec)
                {
                    boost::asio::io_service & ios = pipe->get_io_service();
                    ios.post([pipe]
                              {
                                    boost::system::error_code ec;
                                    pipe->close(ec);
                              });
                };


    };

    template <typename Executor>
    void on_exec_setup(Executor &exec)
    {
        if (!pipe)
            pipe = std::make_shared<boost::process::async_pipe>(get_io_service(exec.seq));

        int res = apply_out_handles(pipe->native_sink(), std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
        if (res == -1)
            exec.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
    }

};

}}}}

#endif
