// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_ASYNC_IN_HPP
#define BOOST_PROCESS_DETAIL_POSIX_ASYNC_IN_HPP

#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/posix/async_handler.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/write.hpp>
#include <memory>


#if defined (BOOST_PROCESS_USE_FUTURE)
#include <future>
#endif



using namespace std;

namespace boost { namespace process { namespace detail { namespace posix {


template<typename Buffer>
struct async_in_buffer : ::boost::process::detail::posix::async_handler
{
    std::shared_ptr<boost::asio::posix::stream_descriptor> stream_descriptor;

    Buffer & buf;

#if defined (BOOST_PROCESS_USE_FUTURE)
    std::shared_ptr<std::promise<void>> promise;
    async_in_buffer operator<(std::future<void> & fut)
    {
        promise = std::make_shared<std::promise<void>>();
        fut = promise->get_future(); return std::move(*this);
    }
#endif

    std::shared_ptr<boost::process::pipe> pipe = std::make_shared<boost::process::pipe>(pipe::create_async());
    //because the pipe will be moved later on, but i might need the source at another point.
    boost::iostreams::file_descriptor_source source = pipe->source();

    async_in_buffer(Buffer & buf) : buf(buf)
    {
    }
    template <typename Executor>
    inline void on_success(Executor &exec) const
    {
        pipe->source().close();
        boost::asio::io_service &is_ser = get_io_service(exec.seq);


        auto& stream_descriptor = this->stream_descriptor;
        auto  pipe              = this->pipe;

#if defined (BOOST_PROCESS_USE_FUTURE)
        if (this->promise)
        {
            auto promise = this->promise;

            boost::asio::async_write(*stream_descriptor, buf,
                [promise](const boost::system::error_code & ec, std::size_t)
                {
                    std::error_code e(ec.value(), std::system_category());
                    promise->set_exception(std::make_exception_ptr(std::system_error(e)));
                    promise->set_value();
                });
        }
        else
#endif

        boost::asio::async_write(*stream_descriptor, buf,
                [stream_descriptor, pipe](const boost::system::error_code&ec, std::size_t size){});
    }

    template<typename Executor>
    std::function<void(const std::error_code&)> on_exit_handler(Executor & exec)
    {

        stream_descriptor = std::make_shared<boost::asio::posix::stream_descriptor>(get_io_service(exec.seq), pipe->sink().handle());

        auto stream_descriptor = this->stream_descriptor;
        auto pipe = this->pipe;
        return [stream_descriptor, pipe](const std::error_code& ec)
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
        ::close(STDIN_FILENO);
        ::dup2(source.handle(), STDIN_FILENO);
        pipe->sink().close();

    }
};


}}}}

#endif
