
// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_PROCESS_HANDLE_FD_OR_SIGNAL_HPP
#define BOOST_PROCESS_V2_DETAIL_PROCESS_HANDLE_FD_OR_SIGNAL_HPP

#include <boost/process/v2/detail/config.hpp>

#include <sys/types.h>
#include <sys/wait.h>

#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/exit_code.hpp>
#include <boost/process/v2/pid.hpp>

#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/any_io_executor.hpp>
#include <asio/append.hpp>
#include <asio/associated_immediate_executor.hpp>
#include <asio/compose.hpp>
#include <asio/dispatch.hpp>
#include <asio/posix/basic_stream_descriptor.hpp>
#include <asio/post.hpp>
#if !defined(BOOST_PROCESS_V2_DISABLE_SIGNALSET)
#include <asio/signal_set.hpp>
#endif
#else
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/append.hpp>
#include <boost/asio/associated_immediate_executor.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/posix/basic_stream_descriptor.hpp>
#include <boost/asio/post.hpp>
#if !defined(BOOST_PROCESS_V2_DISABLE_SIGNALSET)
#include <boost/asio/signal_set.hpp>
#endif
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

template<typename Executor = net::any_io_executor>
struct basic_process_handle_fd_or_signal
{
    using native_handle_type = int;
    
    typedef Executor executor_type;

    executor_type get_executor()
    { return descriptor_.get_executor(); }

    /// Rebinds the process_handle to another executor.
    template<typename Executor1>
    struct rebind_executor
    {
        /// The socket type when rebound to the specified executor.
        typedef basic_process_handle_fd_or_signal<Executor1> other;
    };

    template<typename ExecutionContext>
    basic_process_handle_fd_or_signal(ExecutionContext &context,
                                typename std::enable_if<
                                        std::is_convertible<ExecutionContext &,
                                                net::execution_context &>::value
                                >::type * = nullptr)
            : pid_(-1), descriptor_(context)
    {
    }

    template<typename ExecutionContext>
    basic_process_handle_fd_or_signal(ExecutionContext &context,
                                      pid_type pid,
                                      typename std::enable_if<
                                          std::is_convertible<ExecutionContext &,
                                              net::execution_context &>::value
                                          >::type * = nullptr)
            : pid_(pid), descriptor_(context)
    {
    }
    
    template<typename ExecutionContext>
    basic_process_handle_fd_or_signal(ExecutionContext &context,
                                      pid_type pid, native_handle_type process_handle,
                                      typename std::enable_if<
                                          std::is_convertible<ExecutionContext &,
                                              net::execution_context &>::value
                                          >::type * = nullptr)
            : pid_(pid), descriptor_(context, process_handle)
    {
    }
    

    basic_process_handle_fd_or_signal(Executor executor)
            : pid_(-1), descriptor_(executor)
    {
    }

    basic_process_handle_fd_or_signal(Executor executor, pid_type pid)
            : pid_(pid), descriptor_(executor)
    {
    }

    basic_process_handle_fd_or_signal(Executor executor, pid_type pid, native_handle_type process_handle)
            : pid_(pid), descriptor_(executor, process_handle)
    {
    }


    basic_process_handle_fd_or_signal(basic_process_handle_fd_or_signal &&handle)
            : pid_(handle.pid_), descriptor_(std::move(handle.descriptor_))
    {
        handle.pid_ = -1;
    }
    // Warn: does not change the executor of the signal-set.
    basic_process_handle_fd_or_signal& operator=(basic_process_handle_fd_or_signal &&handle)
    {
        pid_ = handle.pid_;
        descriptor_ = std::move(handle.descriptor_);
        handle.pid_ = -1;
        return *this;
    }


    template<typename Executor1>
    basic_process_handle_fd_or_signal(basic_process_handle_fd_or_signal<Executor1> &&handle)
            : pid_(handle.pid_), descriptor_(std::move(handle.descriptor_))
    {
        handle.pid_ = -1;
    }

    pid_type id() const
    { return pid_; }
    native_handle_type native_handle() {return pid_;}

    void terminate_if_running(error_code &)
    {
        if (pid_ <= 0)
            return;
        if (::waitpid(pid_, nullptr, WNOHANG) == 0)
        {
            ::kill(pid_, SIGKILL);
            ::waitpid(pid_, nullptr, 0);
        }
    }

    void terminate_if_running()
    {
        if (pid_ <= 0)
            return;
        if (::waitpid(pid_, nullptr, WNOHANG) == 0)
        {
            ::kill(pid_, SIGKILL);
            ::waitpid(pid_, nullptr, 0);
        }
    }

    void wait(native_exit_code_type &exit_status, error_code &ec)
    {
        if (pid_ <= 0)
            return;
        while (::waitpid(pid_, &exit_status, 0) < 0)
        {
            if (errno != EINTR)
            {
                ec = get_last_error();
                break;
            }               
        }
            
    }

    void wait(native_exit_code_type &exit_status)
    {
        if (pid_ <= 0)
            return;
        error_code ec;
        wait(exit_status, ec);
        if (ec)
            detail::throw_error(ec, "wait(pid)");
    }

    void interrupt(error_code &ec)
    {
        if (pid_ <= 0)
            return;
        if (::kill(pid_, SIGINT) == -1)
            ec = get_last_error();
    }

    void interrupt()
    {
        if (pid_ <= 0)
            return;
        error_code ec;
        interrupt(ec);
        if (ec)
            detail::throw_error(ec, "interrupt");
    }

    void request_exit(error_code &ec)
    {
        if (pid_ <= 0)
            return;
        if (::kill(pid_, SIGTERM) == -1)
            ec = get_last_error();
    }

    void request_exit()
    {
        if (pid_ <= 0)
            return;
        error_code ec;
        request_exit(ec);
        if (ec)
            detail::throw_error(ec, "request_exit");
    }
    
    void suspend()
    {
        if (pid_ <= 0)
            return ;
        error_code ec;
        suspend(ec);
        if (ec)
            detail::throw_error(ec, "suspend");
    }

    void suspend(error_code &ec)
    {
        if (pid_ <= 0)
            return ;
        if (::kill(pid_, SIGSTOP) == -1)
            ec = get_last_error();
    }

    void resume()
    {
        if (pid_ <= 0)
            return ;
        error_code ec;
        resume(ec);
        if (ec)
            detail::throw_error(ec, "resume");
    }

    void resume(error_code &ec)
    {
        if (pid_ <= 0)
            return ;
        if (::kill(pid_, SIGCONT) == -1)
            ec = get_last_error();
    }

    void terminate(native_exit_code_type &exit_status, error_code &ec)
    {
        if (pid_ <= 0)
            return;
        if (::kill(pid_, SIGKILL) == -1)
            ec = get_last_error();
        else
            wait(exit_status, ec);
    }

    void terminate(native_exit_code_type &exit_status)
    {
        if (pid_ <= 0)
            return;
        error_code ec;
        terminate(exit_status, ec);
        if (ec)
            detail::throw_error(ec, "terminate");
    }

    bool running(native_exit_code_type &exit_code, error_code & ec)
    {
        if (pid_ <= 0)
            return false;
        int code = 0;
        int res = ::waitpid(pid_, &code, WNOHANG);
        if (res == -1)
            ec = get_last_error();
        else if (res == 0)
            return true;
        else
        {
            ec.clear();
            exit_code = code;
        }
        return false;
    }

    bool running(native_exit_code_type &exit_code)
    {
        if (pid_ <= 0)
            return false;

        error_code ec;
        bool res = running(exit_code, ec);
        if (ec)
            detail::throw_error(ec, "is_running");
        return res;
    }

    bool is_open() const
    {
        return pid_ != -1;
    }

  private:
    template<typename>
    friend
    struct basic_process_handle_fd_or_signal;
    pid_type pid_ = -1;
    net::posix::basic_stream_descriptor<Executor> descriptor_;
#if !defined(BOOST_PROCESS_V2_DISABLE_SIGNALSET)
    net::basic_signal_set<Executor> signal_set_{descriptor_.get_executor(), SIGCHLD};
#else
    int signal_set_;
#endif
    struct async_wait_op_
    {
        net::posix::basic_descriptor<Executor> &descriptor;
#if !defined(BOOST_PROCESS_V2_DISABLE_SIGNALSET)
        net::basic_signal_set<Executor> &handle;
#else
        int dummy;
#endif
        pid_type pid_;
        native_exit_code_type & exit_code;
        bool needs_post = true;

        template<typename Self>
        void operator()(Self && self)
        {
          self.reset_cancellation_state(asio::enable_total_cancellation());
          (*this)(std::move(self), error_code{});
        }

        template<typename Self>
        void operator()(Self &&self, error_code ec, int = 0)
        {
            int wait_res = -1;
            if (pid_ <= 0) // error, complete early
                ec = net::error::bad_descriptor;
            else if (process_is_running(exit_code))
            {
                wait_res = ::waitpid(pid_, &exit_code, WNOHANG);
                if (wait_res == -1)
                    ec = get_last_error();
            }

            if (!ec && (wait_res == 0))
            {
                if (descriptor.is_open())
                {
                  needs_post = false;
                  descriptor.async_wait(
                      net::posix::descriptor_base::wait_read,
                      std::move(self));
                  return;
                }
                else
                {
#if !defined(BOOST_PROCESS_V2_DISABLE_SIGNALSET)
                  needs_post = false;
                  handle.async_wait(std::move(self));
                  return;
#else
                  BOOST_PROCESS_V2_ASSIGN_EC(ec, net::error::operation_not_supported);
#endif
                }
            }

            if (needs_post)
            {
              auto exec = net::get_associated_immediate_executor(self, descriptor.get_executor());
              net::dispatch(exec, net::append(std::move(self), exit_code, ec));
            }
            else
            {
              auto exec = net::get_associated_executor(self);
              net::dispatch(exec, net::append(std::move(self), exit_code, ec));
            }
        }
        template<typename Self>
        void operator()(Self &&self, native_exit_code_type code, error_code ec)
        {
          self.complete(ec);
        }
    };
 public:
    template<BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void(error_code))
             WaitHandler = net::default_completion_token_t<executor_type>>
    auto async_wait(native_exit_code_type & exit_code,
                    WaitHandler &&handler = net::default_completion_token_t<executor_type>())
      -> decltype(net::async_compose<WaitHandler, void(error_code)>(
                  async_wait_op_{descriptor_, signal_set_, pid_, exit_code}, handler, descriptor_))
    {
        return net::async_compose<WaitHandler, void(error_code)>(
                async_wait_op_{descriptor_, signal_set_, pid_, exit_code}, handler, descriptor_);
    }
};
}

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_HANDLE_FD_OR_SIGNAL_HPP
