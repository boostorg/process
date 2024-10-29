//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_V2_EXPERIMENTAL_BASIC_PROCESS_STREAM_HPP
#define BOOST_PROCESS_V2_EXPERIMENTAL_BASIC_PROCESS_STREAM_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/experimental/console_size.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <boost/process/v2/experimental/detail/basic_stream_handle.hpp>
#else

#include <termios.h>

#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/append.hpp>
#include <asio/basic_signal_set.hpp>
#include <asio/compose.hpp>
#include <asio/posix/basic_stream_descriptor.hpp>
#else
#include <boost/asio/append.hpp>
#include <boost/asio/basic_signal_set.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/posix/basic_stream_descriptor.hpp>
#endif
#endif


BOOST_PROCESS_V2_BEGIN_NAMESPACE
namespace experimental
{

template<typename Executor = net::any_io_executor>
struct basic_stream
{
  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  /// Rebinds the stream type to another executor.
  template <typename Executor1>
  struct rebind_executor
  {
    /// The stream type when rebound to the specified executor.
    typedef basic_stream<Executor1> other;
  };
#if defined(BOOST_PROCESS_V2_POSIX)
  typedef typename net::posix::basic_stream_descriptor<Executor>::native_handle_type native_handle_type;
#else
  typedef HANDLE native_handle_type;
#endif
  // A process_io is always the lowest layer.
  typedef basic_stream lowest_layer_type;

  /// Construct a stream without opening it.
  /**
   * This constructor creates a stream without opening it.
   *
   * @param ex The I/O executor that the stream will use, by default, to
   * dispatch handlers for any asynchronous operations performed on the
   * stream.
   */
  explicit basic_stream(const executor_type& ex)
      : handle_(ex)
  {
  }

  /// Construct a stream without opening it.
  /**
   * This constructor creates a stream without opening it.
   *
   * @param context An execution context which provides the I/O executor that
   * the stream will use, by default, to dispatch handlers for any
   * asynchronous operations performed on the stream.
   */
  template <typename ExecutionContext>
  explicit basic_stream(ExecutionContext& context,
                            net::constraint_t<
                                net::is_convertible<ExecutionContext&, net::execution_context&>::value,
                                net::defaulted_constraint
                            > = net::defaulted_constraint())
                    : handle_(context)
  {
  }

  /// Construct a stream on an existing native stream.
  /**
   * This constructor creates a stream object to hold an existing native
   * stream.
   *
   * @param ex The I/O executor that the stream will use, by default, to
   * dispatch handlers for any asynchronous operations performed on the
   * stream.
   *
   * @param native_handle A native stream.
   *
   * @throws system_error Thrown on failure.
   */
  basic_stream(const executor_type& ex,
               const native_handle_type& native_handle)
      : handle_(ex)
  {
    assign(native_handle);
  }

  /// Construct a stream on an existing native stream.
  /**
   * This constructor creates a stream object to hold an existing native
   * handle.
   *
   * @param context An execution context which provides the I/O executor that
   * the stream will use, by default, to dispatch handlers for any
   * asynchronous operations performed on the stream.
   *
   * @param native_handle A native handle.
   *
   * @throws system_error Thrown on failure.
   */
  template <typename ExecutionContext>
  basic_stream(ExecutionContext& context,
                  const native_handle_type& native_handle,
                   net::constraint_t<
                      net::is_convertible<ExecutionContext&, net::execution_context&>::value
                  > = 0)
        : handle_(context)
  {
      assign(native_handle);
  }

  /// Move-construct a stream from another.
  /**
   * This constructor moves a stream from one object to another.
   *
   * @param other The other stream object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_stream(const executor_type&)
   * constructor.
   */
  basic_stream(basic_stream&& other) noexcept = default;

  /// Move-assign a stream from another.
  /**
   * This assignment operator moves a stream from one object to another.
   *
   * @param other The other stream object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_stream(const executor_type&)
   * constructor.
   */
  basic_stream& operator=(basic_stream&& other) = default;

  // All process IO streams have access to each other's implementations.
  template <typename Executor1>
  friend class basic_stream;

    /// Move-construct a basic_stream from a stream of another executor
  /// type.
  /**
   * This constructor moves a stream from one object to another.
   *
   * @param other The other basic_stream object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_stream(const executor_type&)
   * constructor.
   */
  template <typename Executor1>
  basic_stream(basic_stream<Executor1>&& other,
       net::constraint_t<
        net::is_convertible<Executor1, Executor>::value,
           net::defaulted_constraint
      > = net::defaulted_constraint())
    : handle_(std::move(other.handle_))
  {
  }

  /// Move-assign a basic_stream from a stream of another executor type.
  /**
   * This assignment operator moves a stream from one object to another.
   *
   * @param other The other basic_stream object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_stream(const executor_type&)
   * constructor.
   */
  template <typename Executor1>
  net::constraint_t<
      net::is_convertible<Executor1, Executor>::value,
      basic_stream&
  > operator=(basic_stream<Executor1> && other)
  {
    handle_ = std::move(other.handle_);
    return *this;
  }

  /// Get the executor associated with the object.
  executor_type get_executor() noexcept
  {
    return handle_.get_executor();
  }

  /// Get a reference to the lowest layer.
  /**
   * This function returns a reference to the lowest layer in a stack of
   * layers. Since a stream cannot contain any further layers, it
   * simply returns a reference to itself.
   *
   * @return A reference to the lowest layer in the stack of layers. Ownership
   * is not transferred to the caller.
   */
  lowest_layer_type& lowest_layer()
  {
    return *this;
  }

  /// Get a const reference to the lowest layer.
  /**
   * This function returns a const reference to the lowest layer in a stack of
   * layers. Since a stream cannot contain any further layers, it
   * simply returns a reference to itself.
   *
   * @return A const reference to the lowest layer in the stack of layers.
   * Ownership is not transferred to the caller.
   */
  const lowest_layer_type& lowest_layer() const
  {
    return *this;
  }

  /// Assign an existing native stream to the stream.
  /*
   * This function opens the stream to hold an existing native stream.
   *
   * @param native_handle A native stream.
   *
   * @throws system_error Thrown on failure.
   */
  void assign(const native_handle_type& native_handle)
  {
    return handle_.assign(native_handle);
  }

  /// Assign an existing native stream to the stream.
  /*
   * This function opens the stream to hold an existing native stream.
   *
   * @param native_handle A native stream.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  void assign(const native_handle_type& native_handle,
                                 error_code& ec)
  {
    handle_.assign(native_handle, ec);
  }

  /// Determine whether the stream is open.
  bool is_open() const
  {
    return handle_.is_open();
  }

  /// Close the stream.
  /**
   * This function is used to close the stream. Any asynchronous read or
   * write operations will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @throws system_error Thrown on failure. Note that, even if
   * the function indicates an error, the underlying stream is closed.
   */
  void close()
  {
   handle_.close();
  }

  /// Close the stream.
  /**
   * This function is used to close the stream. Any asynchronous read or
   * write operations will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any. Note that, even if
   * the function indicates an error, the underlying stream is closed.
   */
  void close(error_code& ec)
  {
    handle_.close(ec);
  }

  /// Get the native stream representation.
  /**
   * This function may be used to obtain the underlying representation of the
   * stream. This is intended to allow access to native stream
   * functionality that is not otherwise provided.
   */
  native_handle_type native_handle()
  {
    return handle_.native_handle();
  }

  /// Release ownership of the native stream implementation.
  /**
   * This function may be used to obtain the underlying representation of the
   * stream. After calling this function, @c is_open() returns false. The
   * caller is responsible for closing the stream.
   *
   * All outstanding asynchronous read or write operations will finish
   * immediately, and the handlers for cancelled operations will be passed the
   * boost::asio::error::operation_aborted error.
   */
  native_handle_type release()
  {
    return handle_.release();
  }

  /// Cancel all asynchronous operations associated with the stream.
  /**
   * This function causes all outstanding asynchronous read or write operations
   * to finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @throws system_error Thrown on failure.
   */
  void cancel()
  {
    handle_.cancel();
  }

  /// Cancel all asynchronous operations associated with the stream.
  /**
   * This function causes all outstanding asynchronous read or write operations
   * to finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  void cancel(error_code& ec)
  {
    handle_.cancel(ec) ;
    if (ec)
      return;
#if defined(BOOST_PROCESS_V2_POSIX)
    sig_winch_.cancel(ec);
#endif
  }


  console_size_t console_size()
  {
   error_code ec;
   auto res = console_size(ec);
   if (ec)
     boost::process::v2::detail::throw_error(ec, "console_size");
   return res;
  }
  console_size_t console_size(error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    winsize ws;
    if (ioctl(handle_.native_handle(), TIOCGWINSZ, &ws))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
    return {ws.ws_col, ws.ws_row};
#else
    CONSOLE_SCREEN_BUFFER_INFO bi;
    if (!GetConsoleScreenBufferInfo(handle_.native_handle(), &bi))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    return console_size_t{static_cast<unsigned short>(bi.dwSize.X),
                          static_cast<unsigned short>(bi.dwSize.Y)};
#endif
  }

  bool is_pty()
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    struct ::termios t;
    return !tcgetattr(handle_.native_handle(), &t);
#else
    return handle_.object.is_open();
#endif
  }

    void set_echo(bool enable, error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    struct ::termios t;
    if (tcgetattr(handle_.native_handle(), &t))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    if (ec)
      return ;

    if (enable)
      t.c_lflag |=  (ECHO | ECHOE | ECHOK | ECHONL);
    else
      t.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);

    if (tcsetattr(handle_.native_handle(), TCSANOW, &t))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#else
    DWORD mode;
    if (!GetConsoleMode(handle_.native_handle(), &mode))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    if (enable)
      mode |= ENABLE_ECHO_INPUT;
    else
      mode &= ~ENABLE_ECHO_INPUT;

    if (!ec && !SetConsoleMode(handle_.native_handle(), mode))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#endif
  }

  void set_echo(bool enable)
  {
    error_code ec;
    set_echo(enable, ec);
    if (ec)
      boost::process::v2::detail::throw_error(ec, "set_echo");
  }

  bool echo(error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    struct ::termios t;
    if (tcgetattr(handle_.native_handle(), &t))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    return (t.c_lflag & (ECHO)) == ECHO;
#else
    DWORD mode;
    if (!GetConsoleMode(handle_.native_handle(), &mode))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    return (mode & ENABLE_ECHO_INPUT) == ENABLE_ECHO_INPUT;
#endif
  }

  bool echo()
  {
    error_code ec;
    auto res = echo(ec);
    if (ec)
      boost::process::v2::detail::throw_error(ec, "echo");
    return res;
  }


  void set_line(bool enable, error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    struct ::termios t;
    if (tcgetattr(handle_.native_handle(), &t))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    if (ec)
      return ;

    if (enable)
      t.c_lflag |=  ICANON;
    else
      t.c_lflag &= ~ICANON;

    if (tcsetattr(handle_.native_handle(), TCSANOW, &t))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#else
    DWORD mode;
    if (!GetConsoleMode(handle_.native_handle(), &mode))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    if (enable)
      mode |= ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT ;
    else
      mode &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT );

    if (!ec && !SetConsoleMode(handle_.native_handle(), mode))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#endif
  }

  void set_line(bool enable)
  {
    error_code ec;
    set_line(enable, ec);
    if (ec)
      boost::process::v2::detail::throw_error(ec, "set_line");
  }

  bool line(error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    struct ::termios t;
    if (tcgetattr(handle_.native_handle(), &t))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    return (t.c_lflag & ICANON) == ICANON;
#else
    DWORD mode;
    if (!GetConsoleMode(handle_.native_handle(), &mode))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    return (mode & ENABLE_LINE_INPUT) == ENABLE_LINE_INPUT;
#endif
  }

  bool line()
  {
    error_code ec;
    auto res = line(ec);
    if (ec)
      boost::process::v2::detail::throw_error(ec, "line");
    return res;
  }

 protected:

  struct async_wait_for_size_change_op_
  {
    basic_stream<Executor> * this_;
    console_size_t buf;
#if defined(BOOST_PROCESS_V2_POSIX)
    template<typename Self>
    void operator()(Self && self)
    {
      if (!this_->is_pty())
      {
        auto e = net::get_associated_immediate_executor(self, this_->sig_winch_.get_executor());
        net::dispatch(e,
            net::append(std::move(self), net::error::operation_not_supported));
      }
      else
        this_->sig_winch_.async_wait(std::move(self));
    }

    template<typename Self>
    void operator()(Self && self, error_code ec, int = 0)
    {
      console_size_t cs;
      if (!ec)
        cs = this_->console_size(ec);
      self.complete(ec, cs);
    }
#else
    template<typename Self>
    void operator()(Self && self)
    {
      if (!this_->is_pty())
      {
        auto e = net::get_associated_immediate_executor(self, this_->handle_.object.get_executor());
        return net::dispatch(e, net::append(std::move(self), net::error::operation_not_supported));
      }
      this_->async_wait(std::move(self));
    }

    template<typename Self>
    void operator()(Self && self, error_code ec)
    {
      if (!ec)
      {
        CONSOLE_SCREEN_BUFFER_INFO bi;
        if (::GetConsoleScreenBufferInfo(this_->handle_.object.native_handle(), &bi))
        {
          this_->handle_.cs_buf_.columns = bi.dwSize.X;
          this_->handle_.cs_buf_.rows = bi.dwSize.Y;
        }
        else
          BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      }

      self.complete(ec, this_->handle_.cs_buf_);
    }
#endif
  };

#if defined(BOOST_PROCESS_V2_POSIX)
  net::posix::basic_stream_descriptor<Executor> handle_;
  net::basic_signal_set<Executor> sig_winch_{handle_.get_executor(), SIGWINCH};
#else
  detail::experimental::basic_stream_handle<Executor> handle_;

#endif
 public:
  template <BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void (error_code, console_size_t))
            WaitHandler = net::default_completion_token_t<executor_type>>
  auto async_wait_for_size_change(WaitHandler && handler = net::default_completion_token_t<executor_type>())
      -> decltype(net::async_compose<WaitHandler, void (error_code, console_size_t)>(
                  async_wait_for_size_change_op_{this}, handler, handle_))
  {
    return net::async_compose<WaitHandler, void (error_code, console_size_t)>(
        async_wait_for_size_change_op_{this}, handler, handle_);
  }

  /// Wait for the stream to become ready to read.
  /**
   * This function is used to perform a blocking wait for a stream to enter
   * a ready to read state.
   *
   * @par Example
   * Waiting for a stream to become readable.
   * @code
   *
   * auto in = boost::process::v2::process_io::std_in();
   * ...
   * in.wait();
   * @endcode
   */
  void wait()
  {

    this->handle_.wait(
#if defined(BOOST_PROCESS_V2_POSIX)
        net::posix::descriptor_base::wait_read
#endif
    );
  }


  /// Wait for the stream to become ready to read.
  /**
   * This function is used to perform a blocking wait for a stream to enter
   * a ready to read state.
   *
   * @par Example
   * Waiting for a stream to become readable.
   * @code
   * error_code ec;
   * auto in = boost::process::v2::process_io::std_in(ec);
   * ...
   * in.wait(ec);
   * @endcode
   */
  void wait(error_code  & ec)
  {
    this->handle_.wait(
#if defined(BOOST_PROCESS_V2_POSIX)
        net::posix::descriptor_base::wait_read,
#endif
        ec);
  }

  /// Asynchronously wait for the stream to become ready to read,.
  /**
   * This function is used to perform an asynchronous wait for a stream to
   * enter a ready to read. It is an initiating
   * function for an @ref asynchronous_operation, and always returns
   * immediately.
   *
   * @param w Specifies the desired stream state.
   *
   * @param token The @ref completion_token that will be used to produce a
   * completion handler, which will be called when the wait completes.
   * Potential completion tokens include @ref use_future, @ref use_awaitable,
   * @ref yield_context, or a function object with the correct completion
   * signature. The function signature of the completion handler must be:
   * @code void handler(
   *   const error_code& error // Result of operation.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the completion handler will not be invoked from within this function.
   * On immediate completion, invocation of the handler will be performed in a
   * manner equivalent to using boost::asio::async_immediate().
   *
   * @par Completion Signature
   * @code void(error_code) @endcode
   *
   * @par Example
   * @code
   * void wait_handler(const error_code& error)
   * {
   *   if (!error)
   *   {
   *     // Wait succeeded.
   *   }
   * }
   *
   * ...
   *
   * boost::process::v2::stream stream(my_context);
   * ...
   * stream.async_wait(
   *     wait_handler);
   * @endcode
   *
   * @par Per-Operation Cancellation
   * This asynchronous operation supports cancellation for the following
   * boost::asio::cancellation_type values:
   *
   * @li @c cancellation_type::terminal
   *
   * @li @c cancellation_type::partial
   *
   * @li @c cancellation_type::total
   */
  template <
      BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void (error_code))
      WaitToken = net::default_completion_token_t<executor_type>>
  auto async_wait(WaitToken&& token = net::default_completion_token_t<executor_type>())
  -> decltype(this->handle_.async_wait(
#if defined(BOOST_PROCESS_V2_POSIX)
      net::posix::descriptor_base::wait_read,
#endif
      std::declval<WaitToken&&>()))
  {
    return this->handle_.async_wait(
#if defined(BOOST_PROCESS_V2_POSIX)
        net::posix::descriptor_base::wait_read,
#endif
        std::forward<WaitToken>(token));
  }

  /// Read some data from the stream.
  /**
   * This function is used to read data from the stream stream. The function
   * call will block until one or more bytes of data has been read successfully,
   * or until an error occurs.
   *
   * @param buffers One or more buffers into which the data will be read.
   *
   * @returns The number of bytes read.
   *
   * @throws boost::system::system_error Thrown on failure. An error code of
   * boost::asio::error::eof indicates that the connection was closed by the
   * peer.
   *
   * @note The read_some operation may not read all of the requested number of
   * bytes. Consider using the @ref read function if you need to ensure that
   * the requested amount of data is read before the blocking operation
   * completes.
   *
   * @par Example
   * To read into a single data buffer use the @ref buffer function as follows:
   * @code
   * stream.read_some(boost::asio::buffer(data, size));
   * @endcode
   * See the @ref buffer documentation for information on reading into multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   */
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers)
  {
    error_code ec;
    auto res = read_some(buffers, ec);
    if (ec)
      detail::throw_error(ec, "read_some");
    return res;
  }

  /// Read some data from the stream.
  /**
   * This function is used to read data from the stream stream. The function
   * call will block until one or more bytes of data has been read successfully,
   * or until an error occurs.
   *
   * @param buffers One or more buffers into which the data will be read.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns The number of bytes read. Returns 0 if an error occurred.
   *
   * @note The read_some operation may not read all of the requested number of
   * bytes. Consider using the @ref read function if you need to ensure that
   * the requested amount of data is read before the blocking operation
   * completes.
   */
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers,
                        boost::system::error_code& ec)
  {
    return this->handle_.read_some(buffers, ec);
  }

  /// Start an asynchronous read.
  /**
   * This function is used to asynchronously read data from the stream
   * stream. It is an initiating function for an @ref
   * asynchronous_operation, and always returns immediately.
   *
   * @param buffers One or more buffers into which the data will be read.
   * Although the buffers object may be copied as necessary, ownership of the
   * underlying memory blocks is retained by the caller, which must guarantee
   * that they remain valid until the completion handler is called.
   *
   * @param token The @ref completion_token that will be used to produce a
   * completion handler, which will be called when the read completes.
   * Potential completion tokens include @ref use_future, @ref use_awaitable,
   * @ref yield_context, or a function object with the correct completion
   * signature. The function signature of the completion handler must be:
   * @code void handler(
   *   const boost::system::error_code& error, // Result of operation.
   *   std::size_t bytes_transferred // Number of bytes read.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the completion handler will not be invoked from within this function.
   * On immediate completion, invocation of the handler will be performed in a
   * manner equivalent to using boost::asio::async_immediate().
   *
   * @par Completion Signature
   * @code void(boost::system::error_code, std::size_t) @endcode
   *
   * @note The read operation may not read all of the requested number of bytes.
   * Consider using the @ref async_read function if you need to ensure that the
   * requested amount of data is read before the asynchronous operation
   * completes.
   *
   * @par Example
   * To read into a single data buffer use the @ref buffer function as follows:
   * @code
   * stream.async_read_some(boost::asio::buffer(data, size), handler);
   * @endcode
   * See the @ref buffer documentation for information on reading into multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   *
   * @par Per-Operation Cancellation
   * This asynchronous operation supports cancellation for the following
   * boost::asio::cancellation_type values:
   *
   * @li @c cancellation_type::terminal
   *
   * @li @c cancellation_type::partial
   *
   * @li @c cancellation_type::total
   */
  template <typename MutableBufferSequence,
            BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void (error_code, std::size_t)) ReadToken =
                                      net::default_completion_token_t<executor_type>>
  auto async_read_some(const MutableBufferSequence& buffers,
                       ReadToken&& token = net::default_completion_token_t<executor_type>())
      -> decltype(this->handle_.async_read_some(buffers, std::forward<ReadToken>(token)))
  {
    return this->handle_.async_read_some(buffers, std::forward<ReadToken>(token));
  }

  /// Write some data to the stream.
  /**
   * This function is used to write data to the stream stream. The function
   * call will block until one or more bytes of the data has been written
   * successfully, or until an error occurs.
   *
   * @param buffers One or more data buffers to be written to the stream.
   *
   * @returns The number of bytes written.
   *
   * @throws boost::system::system_error Thrown on failure. An error code of
   * boost::asio::error::eof indicates that the connection was closed by the
   * peer.
   *
   * @note The write_some operation may not transmit all of the data to the
   * peer. Consider using the @ref write function if you need to ensure that
   * all data is written before the blocking operation completes.
   *
   * @par Example
   * To write a single data buffer use the @ref buffer function as follows:
   * @code
   * stream.write_some(boost::asio::buffer(data, size));
   * @endcode
   * See the @ref buffer documentation for information on writing multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   */
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers)
  {
    error_code ec;
    auto res = write_some(buffers, ec);
    if (ec)
      detail::throw_error(ec, "write_some");
    return res;
  }

  /// Write some data to the stream.
  /**
   * This function is used to write data to the stream stream. The function
   * call will block until one or more bytes of the data has been written
   * successfully, or until an error occurs.
   *
   * @param buffers One or more data buffers to be written to the stream.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns The number of bytes written. Returns 0 if an error occurred.
   *
   * @note The write_some operation may not transmit all of the data to the
   * peer. Consider using the @ref write function if you need to ensure that
   * all data is written before the blocking operation completes.
   */
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers,
      boost::system::error_code& ec)
  {
    return this->handle_.write_some(buffers, ec);
  }

  /// Start an asynchronous write.
  /**
   * This function is used to asynchronously write data to the stream
   * stream. It is an initiating function for an @ref
   * asynchronous_operation, and always returns immediately.
   *
   * @param buffers One or more data buffers to be written to the stream.
   * Although the buffers object may be copied as necessary, ownership of the
   * underlying memory blocks is retained by the caller, which must guarantee
   * that they remain valid until the completion handler is called.
   *
   * @param token The @ref completion_token that will be used to produce a
   * completion handler, which will be called when the write completes.
   * Potential completion tokens include @ref use_future, @ref use_awaitable,
   * @ref yield_context, or a function object with the correct completion
   * signature. The function signature of the completion handler must be:
   * @code void handler(
   *   const boost::system::error_code& error, // Result of operation.
   *   std::size_t bytes_transferred // Number of bytes written.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the completion handler will not be invoked from within this function.
   * On immediate completion, invocation of the handler will be performed in a
   * manner equivalent to using boost::asio::async_immediate().
   *
   * @par Completion Signature
   * @code void(boost::system::error_code, std::size_t) @endcode
   *
   * @note The write operation may not transmit all of the data to the peer.
   * Consider using the @ref async_write function if you need to ensure that all
   * data is written before the asynchronous operation completes.
   *
   * @par Example
   * To write a single data buffer use the @ref buffer function as follows:
   * @code
   * stream.async_write_some(boost::asio::buffer(data, size), handler);
   * @endcode
   * See the @ref buffer documentation for information on writing multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   *
   * @par Per-Operation Cancellation
   * This asynchronous operation supports cancellation for the following
   * boost::asio::cancellation_type values:
   *
   * @li @c cancellation_type::terminal
   *
   * @li @c cancellation_type::partial
   *
   * @li @c cancellation_type::total
   */
  template <typename ConstBufferSequence,
      BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void (boost::system::error_code,
        std::size_t)) WriteToken = net::default_completion_token_t<executor_type>>
  auto async_write_some(const ConstBufferSequence& buffers,
      WriteToken&& token = net::default_completion_token_t<executor_type>())
    -> decltype(this->handle_.async_write_some(buffers, std::forward<WriteToken>(token)))
  {
    return this->handle_.async_write_some(buffers, std::forward<WriteToken>(token));
  }

};


template<typename Executor>
basic_stream<Executor> open_stdin(Executor exec, bool duplicate, error_code & ec)
{
  typename basic_stream<Executor>::native_handle_type handle;
#if defined(BOOST_WINDOWS_API)
  if (!duplicate)
    handle = GetStdHandle(STD_INPUT_HANDLE);
  else if (
      !DuplicateHandle(
          GetCurrentProcess(),
          GetStdHandle(STD_INPUT_HANDLE),
          GetCurrentProcess(),
          &handle,
          0u,
          FALSE,
          DUPLICATE_SAME_ACCESS))
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#else
  handle = duplicate ? dup(STDIN_FILENO) : STDIN_FILENO;
  if (handle < 0)
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#endif
  return basic_stream<Executor>{exec, handle};
}

template<typename Executor>
basic_stream<Executor> open_stdout(Executor exec, bool duplicate, error_code & ec)
{
  typename basic_stream<Executor>::native_handle_type handle;
#if defined(BOOST_WINDOWS_API)
  if (!duplicate)
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
  else if (
      !DuplicateHandle(
          GetCurrentProcess(),
          GetStdHandle(STD_OUTPUT_HANDLE),
          GetCurrentProcess(),
          &handle,
          0u,
          FALSE,
          DUPLICATE_SAME_ACCESS))
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#else
  handle = duplicate ? dup(STDOUT_FILENO) : STDOUT_FILENO;
  if (handle < 0)
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#endif
  return basic_stream<Executor>{exec, handle};
}

template<typename Executor>
basic_stream<Executor> open_stderr(Executor exec, bool duplicate, error_code & ec)
{
  typename basic_stream<Executor>::native_handle_type handle;
#if defined(BOOST_WINDOWS_API)
  if (!duplicate)
    handle = GetStdHandle(STD_ERROR_HANDLE);
  else if (
      !DuplicateHandle(
          GetCurrentProcess(),
          GetStdHandle(STD_ERROR_HANDLE),
          GetCurrentProcess(),
          &handle,
          0u,
          FALSE,
          DUPLICATE_SAME_ACCESS))
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#else
  handle = duplicate ? dup(STDERR_FILENO) : STDERR_FILENO;
  if (handle < 0)
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
#endif
  return basic_stream<Executor>{exec, handle};
}

template<typename Executor>
basic_stream<Executor> open_stdin(Executor exec, bool duplicate = true)
{
  error_code ec;
  auto res = open_stdin(exec, duplicate, ec);
  if (ec)
    detail::throw_error(ec, "open_stdin");
  return res;
}

template<typename Executor>
basic_stream<Executor> open_stdout(Executor exec, bool duplicate = true)
{
  error_code ec;
  auto res = open_stdout(exec, duplicate, ec);
  if (ec)
    detail::throw_error(ec, "open_stdin");
  return res;

}


template<typename Executor>
basic_stream<Executor> open_stderr(Executor exec, bool duplicate = true)
{
  error_code ec;
  auto res = open_stderr(exec, duplicate, ec);
  if (ec)
    detail::throw_error(ec, "open_stderr");
  return res;
}


}
BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_EXPERIMENTAL_BASIC_PROCESS_STREAM_HPP
