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
#include <boost/asio/write.hpp>
#include <boost/process/async_pipe.hpp>
#include <memory>


#include <future>



using namespace std;

namespace boost { namespace process { namespace detail { namespace posix {


template<typename Buffer>
struct async_in_buffer : ::boost::process::detail::posix::async_handler
{
    Buffer & buf;

    std::shared_ptr<std::promise<void>> promise;
    async_in_buffer operator<(std::future<void> & fut)
    {
        promise = std::make_shared<std::promise<void>>();
        fut = promise->get_future(); return std::move(*this);
    }

    std::shared_ptr<boost::process::async_pipe> pipe;

    async_in_buffer(Buffer & buf) : buf(buf)
    {
    }
    template <typename Executor>
    inline void on_success(Executor &exec) const
    {

        auto  pipe              = this->pipe;

        if (this->promise)
        {
            auto promise = this->promise;

            boost::asio::async_write(*pipe, buf,
                [pipe, promise](const boost::system::error_code & ec, std::size_t)
                {
                    std::error_code e(ec.value(), std::system_category());
                    promise->set_exception(std::make_exception_ptr(std::system_error(e)));
                    promise->set_value();
                });
        }
        else

        boost::asio::async_write(*pipe, buf,
                [pipe](const boost::system::error_code&ec, std::size_t size){});
    }

    template<typename Executor>
    std::function<void(int, const std::error_code&)> on_exit_handler(Executor & exec)
    {
        if (!pipe)
            pipe = std::make_shared<boost::process::async_pipe>(get_io_service(exec.seq));

        auto pipe = this->pipe;
        return [pipe](int, const std::error_code& ec)
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

        if (::dup2(pipe->native_source(), STDIN_FILENO) == -1)
            exec.set_error(::boost::process::detail::get_last_error(), "dup2() failed");
    }
};


}}}}

#endif
