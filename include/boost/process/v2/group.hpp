// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_GROUP_HPP
#define BOOST_PROCESS_V2_GROUP_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/default_launcher.hpp>
#include <boost/process/v2/exit_code.hpp>
#include <boost/process/v2/process_handle.hpp>


#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <boost/process/v2/detail/group_impl_windows.hpp>
#else
#include <boost/process/v2/detail/group_impl_posix.hpp>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

/** A process handle is an unmanaged version of a process.
 * This means it does not terminate the proces on destruction and
 * will not keep track of the exit-code. 
 * 
 * Note that the exit code might be discovered early, during a call to `running`.
 * Thus it can only be discovered that process has exited already.
 */
template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor,
         typename Launcher = default_process_launcher>
struct basic_group
{
    /// The native handle of the process. 
    /** This might be undefined on posix systems that only support signals */
#if defined(GENERATING_DOCUMENTATION)
    using native_handle_type = implementation_defined;
#else
    using native_handle_type = typename detail::basic_group_impl<Executor>::native_handle_type;
#endif
    /// The executor_type of the group
    using executor_type = Executor;

    /// The launcher used by the group for emplacing
    using launcher_type = Launcher;

    // Get the native group handle
    native_handle_type native_handle() {return impl_.native_handle();}

    /// Getter for the executor
    executor_type get_executor() { return impl_.get_executor();}

    /// Rebinds the group to another executor.
    template<typename Executor1>
    struct rebind_executor
    {
        /// The socket type when rebound to the specified executor.
        typedef basic_group<Executor1, Launcher> other;
    };


    /// Construct a basic_group from an execution_context.
    /**
    * @tparam ExecutionContext The context must fulfill the asio::execution_context requirements
    */
    template<typename ExecutionContext>
    basic_group(ExecutionContext &context,
                typename std::enable_if<
                    std::is_convertible<ExecutionContext&,
                        BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context&>::value, 
                        launcher_type>::type launcher = launcher_type()) 
                        : basic_group(context.get_executor(), std::move(launcher))
    {}

    /// Construct an empty group from an executor.
    basic_group(executor_type executor, launcher_type launcher = launcher_type()) 
        : impl_(std::move(executor)), launcher_(std::move(launcher)) {}

    /// Construct an empty group from an executor and bind it to a pid and the native-handle
    /** On some non-linux posix systems this overload is not present.
     */
    basic_group(executor_type executor, native_handle_type group, 
                launcher_type launcher = launcher_type()) 
        : impl_(std::move(executor), group), , launcher_(std::move(launcher)) {}

    /// Move construct and rebind the executor.
    template<typename Executor1>
    basic_group(basic_group<Executor1> &&handle) 
        : procs_(std::move(handle.procs_)), impl_(std::move(handle.impl_)),
          launcher_(std::move(handle.launcher_)) {}

    /// wait for one process to exit and store the exit code in exit_status.
    single_process_exit wait_one(error_code &ec)
    {
        single_process_exit res;
        impl_.wait_one(res.pid, res.exit_code, ec);
        return res;
    }
    /// Throwing @overload wait_one(error_code & ec)
    single_process_exit wait_one()
    {
        error_code ec;
        auto res = wait_one(ec);
        detail::throw_error(ec, "wait_one");
        return res;
    }

    /// wait for all processes to exit. Returns the number of processes exited
    void wait_all(error_code &ec)
    {
        impl_.wait_all(ec);
    }
    /// Throwing @overload wait_all(error_code & ec)
    void wait_all()
    {
        error_code ec;
        wait_all(ec);
        detail::throw_error(ec, "wait_all");
    }

    /// Sends the processes a signal to ask for an interrupt, which the process may interpret as a shutdown.
    /** Maybe be ignored by the subprocess. */
    void interrupt(error_code &ec)
    {
        impl_.interrupt(ec);
    }

    /// Throwing @overload void interrupt()
    void interrupt()
    {
        error_code ec;
        interrupt(ec);
        detail::throw_error(ec, "interrupt");
    }

    /// Sends the processes a signal to ask for a graceful shutdown. Maybe be ignored by the subprocess.
    void request_exit(error_code &ec)
    {
        impl_.request_exit(ec);
    }

    /// Throwing @overload void request_exit(error_code & ec)
    void request_exit()
    {
        error_code ec;
        auto res = request_exit(ec);
        detail::throw_error(ec, "request_exit");
    }

    /// Unconditionally terminates the processes and stores the exit code in exit_status.
    void terminate(error_code &ec)
    {
        impl_.terminate(ec);
    }
    /// Throwing @overload void terminate(native_exit_code_type &exit_code, error_code & ec)
    void terminate()
    {
        error_code ec;
        terminate(ec);
        detail::throw_error(ec, "terminate");
    }

    /// Check if the process handle is referring to an existing process.
    bool is_open() const
    {
        return impl_.is_open();
    }

    /// Asynchronously wait for one process to exit and deliver the  exit-code in the completion handler.
    template<BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void(error_code, single_process_exit))
             WaitHandler BOOST_PROCESS_V2_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
    BOOST_PROCESS_V2_INITFN_AUTO_RESULT_TYPE(WaitHandler, void (error_code, single_process_exit))
    async_wait_one(WaitHandler &&handler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
    {
        return impl_.async_wait_one(std::forward<WaitHandler>(handler));
    }

    /// Asynchronously wait for all processes to exit and the gorup to be empty.
    template<BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void(error_code))
             WaitHandler BOOST_PROCESS_V2_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
    BOOST_PROCESS_V2_INITFN_AUTO_RESULT_TYPE(WaitHandler, void (error_code))
    async_wait_all(WaitHandler &&handler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
    {
        return impl_.async_wait_all(std::forward<WaitHandler>(handler));
    }
    /// Throwing overload of @overload contains(pid_type pid, error_code & ec)
    bool contains(pid_type pid)
    {
        error_code ec;
        auto res = contains(ec);
        detail::throw_error(ec, "contains");
        return res;
    }
    /// Check if the pid is in the group
    bool contains(pid_type pid, error_code & ec)
    {
        impl_.contains(std::move(proc), ec);
    }

    template<typename Executor_>
    void add(basic_process_handle<Executor_> proc)
    {
        error_code ec;
        add(proc.id(), proc.native_handle(), ec);
        detail::throw_error(ec, "add");
    }

    template<typename Executor_>
    void add(basic_process_handle<Executor_> proc, error_code & ec)
    {
        impl_.add(std::move(proc), ec);
    }

    template<typename ... Inits>
    pid_type emplace(
        const filesystem::path& exe,
        std::initializer_list<string_view> args,
        Inits&&... inits)
    {
        error_code ec;
        auto ph = launcher_(impl_.get_executor(), exe, std::move(args), 
                            std::forward<Inits>(inits)..., impl_.get_initializer(ec));
        if (ec)
            detail::throw_error(ec);
        return ph.detach().id();
    }

    template<typename ... Inits>
    pid_type emplace(
        const filesystem::path& exe,
        std::initializer_list<wstring_view> args,
        Inits&&... inits)
    {
        error_code ec;
        auto ph = launcher_(impl_.get_executor(), exe, std::move(args), 
                            std::forward<Inits>(inits)..., impl_.get_initializer(ec));
        if (ec)
            detail::throw_error(ec);
        return ph.detach().id();
    }

    template<typename Args, typename ... Inits>
    pid_type emplace(
        const filesystem::path& exe,
        Args && args,
        Inits&&... inits)
    {
        error_code ec;
        auto ph = launcher_(impl_.get_executor(), exe, std::move(args), 
                            std::forward<Inits>(inits)..., impl_.get_initializer(ec));
        if (ec)
            detail::throw_error(ec);
        return ph.detach().id();
    }

    template<typename ... Inits>
    pid_type emplace(
        error_code & ec, 
        const filesystem::path& exe,
        std::initializer_list<string_view> args,
        Inits&&... inits)
    {
        auto ph = launcher_(impl_.get_executor(), ec, exe, std::move(args),
                            std::forward<Inits>(inits)..., impl_.get_initializer(ec));
        if (ec)
            return pid_type{-1};
        return ph.detach().id();
    }

    template<typename ... Inits>
    pid_type emplace(
        error_code & ec, 
        const filesystem::path& exe,
        std::initializer_list<wstring_view> args,
        Inits&&... inits)
    {
        auto ph = launcher_(impl_.get_executor(), ec, exe, std::move(args), 
                            std::forward<Inits>(inits)..., impl_.get_initializer(ec));
        if (ec)
            return pid_type{-1};
        return ph.detach().id();
    }

    template<typename Args, typename ... Inits>
    pid_type emplace(
        error_code & ec, 
        const filesystem::path& exe,
        Args && args,
        Inits&&... inits)
    {
        auto ph = launcher_(impl_.get_executor(), ec, exe, std::move(args), 
                            std::forward<Inits>(inits)..., impl_.get_initializer(ec));
        if (ec)
            return pid_type{-1};
        return ph.detach().id();
    }
    
    void detach() { impl_.detach(); }
  private:
    detail::basic_group_impl<Executor> impl_;
    launcher_type launcher_;
};

/// Process group with the default executor.
using group = basic_group<>;

#if !defined(BOOST_PROCESS_V2_HEADER_ONLY)
extern template struct basic_group<BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor, default_process_launcher>;
#endif

BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)

#include <boost/process/v2/impl/group.ipp>

#endif




#endif //BOOST_PROCESS_V2_GROUP_HPP
