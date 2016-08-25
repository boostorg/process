// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_IO_SERVICE_REF_HPP_
#define BOOST_PROCESS_POSIX_IO_SERVICE_REF_HPP_

#include <boost/process/detail/posix/handler.hpp>
#include <boost/process/detail/posix/async_handler.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/transformation/filter_if.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/fusion/view/transform_view.hpp>
#include <boost/fusion/container/vector/convert.hpp>


#include <functional>
#include <type_traits>
#include <memory>
#include <vector>
#include <sys/wait.h>


namespace boost { namespace process { namespace detail { namespace posix {

template<typename Executor>
struct on_exit_handler_transformer
{
    Executor & exec;
    on_exit_handler_transformer(Executor & exec) : exec(exec) {}
    template<typename Sig>
    struct result;

    template<typename T>
    struct result<on_exit_handler_transformer<Executor>(T&)>
    {
        typedef typename T::on_exit_handler_t type;
    };

    template<typename T>
    auto operator()(T& t) const -> typename T::on_exit_handler_t
    {
        return t.on_exit_handler(exec);
    }
};

template<typename Executor>
struct async_handler_collector
{
    Executor & exec;
    std::vector<std::function<void(int, const std::error_code & ec)>> &handlers;


    async_handler_collector(Executor & exec,
            std::vector<std::function<void(int, const std::error_code & ec)>> &handlers)
                : exec(exec), handlers(handlers) {}

    template<typename T>
    void operator()(T & t) const
    {
        handlers.push_back(t.on_exit_handler(exec));
    };
};

//Also set's up waiting for the exit, so it can close async stuff.
struct io_service_ref : handler_base_ext
{
    io_service_ref(boost::asio::io_service & ios) : ios(ios)
    {

    }
    boost::asio::io_service &get() {return ios;};
    template <class Executor>
    void on_setup(Executor& exec) const
    {
          //must be on the heap so I can move it into the lambda.
          auto asyncs = boost::fusion::filter_if<
                          is_async_handler<
                          typename std::remove_reference< boost::mpl::_ > ::type
                          >>(exec.seq);

          std::vector<std::function<void(int, const std::error_code & ec)>> funcs;
          funcs.reserve(boost::fusion::size(asyncs));
          boost::fusion::for_each(asyncs, async_handler_collector<Executor>(exec, funcs));



          wait_handler wh(std::move(funcs), ios, exec.exit_status);

          auto signal_p = wh.signal_.get();
          signal_p->async_wait(std::move(wh));
    }
    struct wait_handler
    {
        std::shared_ptr<boost::asio::signal_set> signal_;
        std::vector<std::function<void(int, const std::error_code & ec)>> funcs;
        std::shared_ptr<std::atomic<int>> exit_status;

        wait_handler(const wait_handler & ) = default;
        wait_handler(wait_handler && )      = default;
        wait_handler(
                std::vector<std::function<void(int, const std::error_code & ec)>> && funcs,
                boost::asio::io_service & ios,
                const std::shared_ptr<std::atomic<int>> &exit_status)
            : signal_(new boost::asio::signal_set(ios, SIGCHLD)),
              funcs(std::move(funcs)),
              exit_status(exit_status)


        {

        }
        void operator()(const boost::system::error_code & ec_in, int /*signal*/)
        {
            int status;
            ::wait(&status);

            std::error_code ec(ec_in.value(), std::system_category());
            int val = WEXITSTATUS(status);
            exit_status->store(val);

            for (auto & func : funcs)
                func(val, ec);
        }

    };

    // Posix specific notifies
    template <class PosixExecutor>
    void on_fork_setup(PosixExecutor&) const
    {
        ios.notify_fork(boost::asio::io_service::fork_prepare);
    }

    template <class PosixExecutor>
    void on_fork_success(PosixExecutor&) const
    {
        ios.notify_fork(boost::asio::io_service::fork_parent);
    }

    template <class PosixExecutor>
    void on_exec_setup(PosixExecutor&) const
    {
        ios.notify_fork(boost::asio::io_service::fork_child);
    }

private:
    boost::asio::io_service &ios;
};

}}}}

#endif /* BOOST_PROCESS_WINDOWS_IO_SERVICE_REF_HPP_ */
