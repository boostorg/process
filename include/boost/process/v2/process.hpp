// Copyright (c) 2021 Klemens D. Morgenstern (klemens dot morgenstern at gmx dot net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// process.hpp
// ~~~~~~~~~~~~~~
//

#ifndef BOOST_PROCESS_V2_PROCESS_HPP
#define BOOST_PROCESS_V2_PROCESS_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/default_launcher.hpp>
#include <boost/process/v2/exit_code.hpp>
#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/process_handle.hpp>

#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/any_io_executor.hpp>
#else
#include <boost/asio/any_io_executor.hpp>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE


template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>
struct basic_process
{
  using executor_type = Executor;
  executor_type get_executor() {return process_handle_.get_executor();}

  /// Provides access to underlying operating system facilities
  using native_handle_type = typename basic_process_handle<executor_type>::native_handle_type;

  /// Rebinds the process_handle to another executor.
  template <typename Executor1>
  struct rebind_executor
  {
    /// The socket type when rebound to the specified executor.
    typedef basic_process<Executor1> other;
  };

  /** An empty process is similar to a default constructed thread. It holds an empty
  handle and is a place holder for a process that is to be launched later. */
  basic_process() = default;

  basic_process(const basic_process&) = delete;
  basic_process& operator=(const basic_process&) = delete;

  basic_process(basic_process&& lhs) 
    : attached_(lhs.attached_), 
      terminated_(lhs.terminated_), 
      exit_status_{lhs.exit_status_}, 
      process_handle_(std::move(lhs.process_handle_))

  {
    lhs.attached_ = false;
  }
  basic_process& operator=(basic_process&& lhs)
  {
    attached_ = lhs.attached_;
    terminated_ = lhs.terminated_;
    exit_status_ = lhs.exit_status_;
    process_handle_ = std::move(lhs.process_handle_);
    lhs.attached_ = false;
    return *this;
  }

  /// Construct a child from a property list and launch it.
  template<typename ... Inits>
  explicit basic_process(
      executor_type executor,
      const filesystem::path& exe,
      std::initializer_list<string_view> args,
      Inits&&... inits)
      : basic_process(default_process_launcher()(std::move(executor), exe, args, std::forward<Inits>(inits)...))
  {
  }
    /// Construct a child from a property list and launch it.
  template<typename ... Inits>
  explicit basic_process(
      executor_type executor,
      const filesystem::path& exe,
      std::initializer_list<wstring_view> args,
      Inits&&... inits)
      : basic_process(default_process_launcher()(std::move(executor), exe, args, std::forward<Inits>(inits)...))
  {
  }

  /// Construct a child from a property list and launch it.
  template<typename Args, typename ... Inits>
  explicit basic_process(
      executor_type executor,
      const filesystem::path& exe,
      Args&& args, Inits&&... inits)
      : basic_process(default_process_launcher()(std::move(executor), exe,
                                               std::forward<Args>(args), std::forward<Inits>(inits)...))
  {
  }

  /// Construct a child from a property list and launch it.
  template<typename ExecutionContext, typename ... Inits>
  explicit basic_process(
      ExecutionContext & context,
      typename std::enable_if<
          std::is_convertible<ExecutionContext&, 
                              BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value,
          const filesystem::path&>::type exe,
      std::initializer_list<string_view> args,
      Inits&&... inits)
      : basic_process(default_process_launcher()(executor_type(context.get_executor()), exe, args, std::forward<Inits>(inits)...))
  {
  }

    /// Construct a child from a property list and launch it.
  template<typename ExecutionContext, typename ... Inits>
  explicit basic_process(
      ExecutionContext & context,
      typename std::enable_if<
          std::is_convertible<ExecutionContext&, 
                              BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value,
          const filesystem::path&>::type exe,
      std::initializer_list<wstring_view> args,
      Inits&&... inits)
      : basic_process(default_process_launcher()(executor_type(context.get_executor()), exe, args, std::forward<Inits>(inits)...))
  {
  }

  /// Construct a child from a property list and launch it.
  template<typename ExecutionContext, typename Args, typename ... Inits>
  explicit basic_process(
      ExecutionContext & context,
      typename std::enable_if<
          std::is_convertible<ExecutionContext&, 
                              BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value,
          const filesystem::path&>::type exe,
      Args&& args, Inits&&... inits)
      : basic_process(default_process_launcher()(executor_type(context.get_executor()),
       exe, std::forward<Args>(args), std::forward<Inits>(inits)...))
  {
  }

  /// Attach to an existing process
  explicit basic_process(executor_type exec, pid_type pid) : process_handle_{std::move(exec), pid} {}

  /// Attach to an existing process and the internal handle
  explicit basic_process(executor_type exec, pid_type pid, native_handle_type native_handle)
        : process_handle_{std::move(exec), pid, native_handle} {}

  /// Create an invalid handle
  explicit basic_process(executor_type exec) : process_handle_{std::move(exec)} {}

  /// Attach to an existing process
  template <typename ExecutionContext>
  explicit basic_process(ExecutionContext & context, pid_type pid,
                         typename std::enable_if<
                             std::is_convertible<ExecutionContext&,
                                BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value, void *>::type = nullptr)
       : process_handle_{context, pid} {}

  /// Attach to an existing process and the internal handle
  template <typename ExecutionContext>
  explicit basic_process(ExecutionContext & context, pid_type pid, native_handle_type native_handle,
                         typename std::enable_if<
                            std::is_convertible<ExecutionContext&, 
                                BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value, void *>::type = nullptr)
      : process_handle_{context, pid, native_handle} {}

  /// Create an invalid handle
  template <typename ExecutionContext>
  explicit basic_process(ExecutionContext & context,
                         typename std::enable_if<
                             is_convertible<ExecutionContext&, 
                                BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value, void *>::type = nullptr)
     : process_handle_{context} {}



  // tbd behavior
  ~basic_process()
  {
    if (attached_ && !terminated_)
      process_handle_.terminate_if_running();
  }

  void interrupt()
  {
    error_code ec;
    interrupt(ec);
    if (ec)
      throw system_error(ec, "interrupt failed");

  }
  void interrupt(error_code & ec)
  {
    process_handle_.interrupt(ec);
  }

  void request_exit()
  {
    error_code ec;
    request_exit(ec);
    if (ec)
      throw system_error(ec, "request_exit failed");
  }
  void request_exit(error_code & ec)
  {
    process_handle_.request_exit(ec);
  }

  void terminate()
  {
    error_code ec;
    terminate(ec);
    if (ec)
      detail::throw_error(ec, "terminate failed");
  }
  void terminate(error_code & ec)
  {
    process_handle_.terminate(exit_status_, ec);
  }

  int wait()
  {
    error_code ec;
    if (running(ec))
      wait(ec);
    if (ec)
      detail::throw_error(ec, "wait failed");
    return exit_code();
  }
  int wait(error_code & ec)
  {
    if (running(ec))
        process_handle_.wait(exit_status_, ec);
    return exit_code();
  }

  void detach()
  {
    attached_ = false;
  }
  void join() {wait();}
  bool joinable() {return attached_ && process_handle_.is_open(); }

  native_handle_type native_handle() {return process_handle_.native_handle(); }
  int exit_code() const
  {
    return evaluate_exit_code(exit_status_);
  }

  pid_type id() const {return process_handle_.id();}

  native_exit_code_type native_exit_code() const
  {
    return exit_status_;
  }
  
  bool running()
  {
    error_code ec;
    native_exit_code_type exit_code;
    auto r =  process_handle_.running(exit_code, ec);
    if (!ec)
      exit_status_ = exit_code;
    else
      throw system_error(ec, "running failed");

    return r;
  }
  bool running(error_code & ec) noexcept
  {
    native_exit_code_type exit_code ;
    auto r =  process_handle_.running(exit_code, ec);
    if (!ec)
      exit_status_ = exit_code;
    return r;
  }

  bool is_open() const { return process_handle_.is_open(); }
  explicit operator bool() const {return valid(); }

  
  template <BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void (error_code, int))
            WaitHandler BOOST_PROCESS_V2_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
  BOOST_PROCESS_V2_INITFN_AUTO_RESULT_TYPE(WaitHandler, void (error_code, int))
  async_wait(WaitHandler && handler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
  {
    return BOOST_PROCESS_V2_ASIO_NAMESPACE::async_compose<WaitHandler, void (error_code, int)>(
      async_wait_op_{process_handle_, exit_status_}, handler, process_handle_);
  }

private:
  basic_process_handle<Executor> process_handle_;
  bool attached_{true};
  bool terminated_{false};
  native_exit_code_type exit_status_{detail::still_active};

  
  struct async_wait_op_
  {
    basic_process_handle<Executor> & handle;
    native_exit_code_type & res;

    template<typename Self>
    void operator()(Self && self)
    {
      handle.async_wait(std::move(self));
    }

    template<typename Self>
    void operator()(Self && self, error_code ec, native_exit_code_type code)
    {
      if (!ec && process_is_running(code))
        handle.async_wait(std::move(self));
      else
      {
        if (!ec)
          res = code;
        std::move(self).complete(ec, evaluate_exit_code(code));
      }
    }
  };
};


typedef basic_process<> process;

BOOST_PROCESS_V2_END_NAMESPACE


#endif //BOOST_PROCESS_V2_PROCESS_HPP