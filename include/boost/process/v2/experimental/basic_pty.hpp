//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_VS_EXPERIMENTAL_BASIC_PTY_HPP
#define BOOST_PROCESS_VS_EXPERIMENTAL_BASIC_PTY_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/experimental/console_size.hpp>

#define NTDDI_VERSION 0x0A000006
#if defined(BOOST_PROCESS_V2_WINDOWS) && !defined(NTDDI_VERSION)
#include <sdkddkver.h>
#endif

#if !defined(BOOST_PROCESS_V2_WINDOWS) || (NTDDI_VERSION >= 0x0A000006)

#if defined(BOOST_PROCESS_V2_WINDOWS)
#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/basic_readable_pipe.hpp>
#include <asio/basic_writable_pipe.hpp>
#include <asio/connect_pipe.hpp>
#else
#include <boost/asio/basic_readable_pipe.hpp>
#include <boost/asio/basic_writable_pipe.hpp>
#include <boost/asio/connect_pipe.hpp>
#endif
#include <boost/process/v2/windows/default_launcher.hpp>
#else
#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/posix/basic_stream_descriptor.hpp>
#else
#include <boost/asio/posix/basic_stream_descriptor.hpp>
#endif
#include <boost/process/v2/posix/default_launcher.hpp>
#endif



BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace experimental
{


template<typename Executor = net::any_io_executor>
class basic_pty
{
#if defined(BOOST_PROCESS_V2_WINDOWS)
  struct handle_t_
  {
    net::basic_readable_pipe<Executor> rm, rs{rm.get_executor()};
    net::basic_writable_pipe<Executor> wm{rm.get_executor()}, ws{rm.get_executor()};
    template<typename Arg>
    handle_t_(Arg && arg) : rm{arg} {}

    bool is_open() {return rm.is_open();}
  };

  handle_t_ handle_;
  net::basic_readable_pipe<Executor> &  read_handle_() {return handle_.rm;}
  net::basic_writable_pipe<Executor> & write_handle_() {return handle_.wm;}
  struct hcon_deleter_
  {
    void operator()(HPCON con)
    {
      ClosePseudoConsole(con);
    }
  };
  std::unique_ptr<void, hcon_deleter_> con_;

#else
  net::posix::basic_stream_descriptor<Executor> handle_;

  net::posix::basic_stream_descriptor<Executor> & read_handle_() { return handle_; };
  net::posix::basic_stream_descriptor<Executor> & write_handle_() { return handle_; };
#endif
 public:
  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  /// Rebinds the descriptor type to another executor.
  template <typename Executor1>
  struct rebind_executor
  {
    /// The descriptor type when rebound to the specified executor.
    typedef basic_pty<Executor1> other;
  };

  /// A descriptor is always the lowest layer.
  typedef basic_pty lowest_layer_type ;


  /// Construct a stream descriptor without opening it.
  /**
   * This constructor creates a stream descriptor without opening it. The
   * descriptor needs to be opened and then connected or accepted before data
   * can be sent or received on it.
   *
   * @param ex The I/O executor that the descriptor will use, by default, to
   * dispatch handlers for any asynchronous operations performed on the
   * descriptor.
   */
  explicit basic_pty(const executor_type& ex)
    : handle_(ex)
  {
  }

  /// Construct a stream descriptor without opening it.
  /**
   * This constructor creates a stream descriptor without opening it. The
   * descriptor needs to be opened and then connected or accepted before data
   * can be sent or received on it.
   *
   * @param context An execution context which provides the I/O executor that
   * the descriptor will use, by default, to dispatch handlers for any
   * asynchronous operations performed on the descriptor.
   */
  template <typename ExecutionContext>
  explicit basic_pty(ExecutionContext& context,
                     net::constraint_t<
                      std::is_convertible<ExecutionContext&, net::execution_context&>::value,
                      net::defaulted_constraint
                     > = net::defaulted_constraint())
        : handle_(context)
  {
  }

  /// Move-construct a stream descriptor from another.
  /**
   * This constructor moves a stream descriptor from one object to another.
   *
   * @param other The other stream descriptor object from which the move
   * will occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_pty(const executor_type&)
   * constructor.
   */
  basic_pty(basic_pty&& other) noexcept
    : handle_(std::move(other))
  {
  }

  /// Move-assign a stream descriptor from another.
  /**
   * This assignment operator moves a stream descriptor from one object to
   * another.
   *
   * @param other The other stream descriptor object from which the move
   * will occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_pty(const executor_type&)
   * constructor.
   */
  basic_pty& operator=(basic_pty&& other)
  {
    handle_= std::move(other.handle_);
    return *this;
  }

  /// Move-construct a basic_pty from a descriptor of another
  /// executor type.
  /**
   * This constructor moves a descriptor from one object to another.
   *
   * @param other The other basic_pty object from which the move
   * will occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_pty(const executor_type&)
   * constructor.
   */
  template <typename Executor1>
  basic_pty(basic_pty<Executor1>&& other,
      net::constraint_t<
          net::is_convertible<Executor1, Executor>::value,
          net::defaulted_constraint
      > = net::defaulted_constraint())
    : handle_(std::move(other))
  {
  }

  /// Move-assign a basic_pty from a descriptor of another
  /// executor type.
  /**
   * This assignment operator moves a descriptor from one object to another.
   *
   * @param other The other basic_pty object from which the move
   * will occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_pty(const executor_type&)
   * constructor.
   */
  template <typename Executor1>
  net::constraint_t<
      net::is_convertible<Executor1, Executor>::value,
    basic_pty&
  > operator=(basic_pty<Executor1> && other)
  {
    handle_ = std::move(other.handle_);
    return *this;
  }


  /// Get the executor associated with the object.
  const executor_type& get_executor() noexcept
  {
    return handle_.get_executor();
  }

  /// Get a reference to the lowest layer.
  /**
   * This function returns a reference to the lowest layer in a stack of
   * layers. Since a descriptor cannot contain any further layers, it
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
   * layers. Since a descriptor cannot contain any further layers, it
   * simply returns a reference to itself.
   *
   * @return A const reference to the lowest layer in the stack of layers.
   * Ownership is not transferred to the caller.
   */
  const lowest_layer_type& lowest_layer() const
  {
    return *this;
  }

  /// Determine whether the descriptor is open.
  bool is_open() const
  {
    return handle_.is_open();
  }

  /// Close the descriptor.
  /**
   * This function is used to close the descriptor. Any asynchronous read or
   * write operations will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure. Note that, even if
   * the function indicates an error, the underlying descriptor is closed.
   */
  void close()
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    handle_.close();
#else
    handle_.rm.close();
    handle_.wm.close();
#endif
  }

  /// Close the descriptor.
  /**
   * This function is used to close the descriptor. Any asynchronous read or
   * write operations will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any. Note that, even if
   * the function indicates an error, the underlying descriptor is closed.
   */
  void close(boost::system::error_code& ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    handle_.close(ec);
#else
    handle_.wm.close(ec);
    handle_.rm.close(ec);
#endif
  }

  /// Cancel all asynchronous operations associated with the descriptor.
  /**
   * This function causes all outstanding asynchronous read or write operations
   * to finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void cancel()
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    handle_.cancel();
#else
    handle_.rm.cancel();
    handle_.wm.cancel();
#endif
  }

  /// Cancel all asynchronous operations associated with the descriptor.
  /**
   * This function causes all outstanding asynchronous read or write operations
   * to finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  void cancel(boost::system::error_code& ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    handle_.cancel(ec);
#else
    handle_.rm.cancel(ec);
    handle_.wm.cancel(ec);
#endif
  }

  /// Write some data to the descriptor.
  /**
   * This function is used to write data to the stream descriptor. The function
   * call will block until one or more bytes of the data has been written
   * successfully, or until an error occurs.
   *
   * @param buffers One or more data buffers to be written to the descriptor.
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
   * descriptor.write_some(boost::asio::buffer(data, size));
   * @endcode
   * See the @ref buffer documentation for information on writing multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   */
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers)
  {
    return write_handle_().write_some(buffers);
  }

  /// Write some data to the descriptor.
  /**
   * This function is used to write data to the stream descriptor. The function
   * call will block until one or more bytes of the data has been written
   * successfully, or until an error occurs.
   *
   * @param buffers One or more data buffers to be written to the descriptor.
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
    return write_handle_().write_some(buffers, ec);
  }

  /// Start an asynchronous write.
  /**
   * This function is used to asynchronously write data to the stream
   * descriptor. It is an initiating function for an @ref
   * asynchronous_operation, and always returns immediately.
   *
   * @param buffers One or more data buffers to be written to the descriptor.
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
   * descriptor.async_write_some(boost::asio::buffer(data, size), handler);
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
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (boost::system::error_code,
        std::size_t)) WriteToken = net::default_completion_token_t<executor_type>>
  auto async_write_some(const ConstBufferSequence& buffers,
      WriteToken&& token = net::default_completion_token_t<executor_type>())
    -> decltype(write_handle_().async_write_some(buffers, std::forward<WriteToken>(token)))
  {
    return write_handle_().async_write_some(buffers, std::forward<WriteToken>(token));
  }

  /// Read some data from the descriptor.
  /**
   * This function is used to read data from the stream descriptor. The function
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
   * descriptor.read_some(boost::asio::buffer(data, size));
   * @endcode
   * See the @ref buffer documentation for information on reading into multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   */
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers)
  {
    return read_handle_().read_some(buffers);
  }

  /// Read some data from the descriptor.
  /**
   * This function is used to read data from the stream descriptor. The function
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
    return read_handle_().read_some(buffers, ec);
  }

  /// Start an asynchronous read.
  /**
   * This function is used to asynchronously read data from the stream
   * descriptor. It is an initiating function for an @ref
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
   * descriptor.async_read_some(boost::asio::buffer(data, size), handler);
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
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (boost::system::error_code,
        std::size_t)) ReadToken = net::default_completion_token_t<executor_type>>
  auto async_read_some(const MutableBufferSequence& buffers,
      ReadToken&& token = net::default_completion_token_t<executor_type>())
    -> decltype(read_handle_().async_read_some(buffers, std::forward<ReadToken>(token)))
  {
    return read_handle_().async_read_some(buffers, std::forward<ReadToken>(token));
  }

  void open(console_size_t size, error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    auto res = posix_openpt(O_RDWR | O_NOCTTY);
    if (res < 0)
    {
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      return ;
    }
    else
      handle_.assign(res, ec);


    winsize ws{size.rows, size.columns};
    if (ioctl(res, TIOCSWINSZ, &ws) != 0)
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

/*    struct termios tios;
    if (!ec && ioctl(res, TCGETS, &tios))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

#if defined (IUTF8)
    tios.c_iflag |= IUTF8;
#endif
    tios.c_lflag &= ~(ECHOE | ECHOK | ECHONL | ICANON);


    if (!ec && ioctl(res, TCSETSW, &tios))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
*/

    if (ec)
      return ;

    if (grantpt(res) || unlockpt(res))
    {
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      this->close(ec);
    }
#else
    net::connect_pipe(handle_.rm, handle_.ws, ec);
    if (!ec)
    {
      net::connect_pipe(handle_.wm, handle_.rs, ec);
      if (ec)
        handle_.rm.close(ec);
    }
    if (ec)
      return ;

    HPCON out;
    auto res = CreatePseudoConsole(
        COORD{static_cast<short>(size.columns), static_cast<short>(size.rows)},
        handle_.rs.native_handle(),
        handle_.ws.native_handle(), 0, &out);
    if (res != S_OK)
      BOOST_PROCESS_V2_ASSIGN_EC(ec, HRESULT_CODE(res), system_category());
    else
      con_.reset(out);
#endif
  }

  void open(console_size_t size)
  {
    error_code ec;
    open(size, ec);
    if (ec)
      boost::process::v2::detail::throw_error(ec, "open");
  }

  void resize(console_size_t size, error_code & ec)
  {
#if defined(BOOST_PROCESS_V2_POSIX)
    winsize ws{size.rows, size.columns, 0, 0};
    if (ioctl(handle_.native_handle(), TIOCSWINSZ, &ws) != 0)
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

#else
    auto res = ::ResizePseudoConsole(con_.get(),
                                     COORD{static_cast<short>(size.columns), static_cast<short>(size.rows)});
    if (res != S_OK)
      BOOST_PROCESS_V2_ASSIGN_EC(ec, HRESULT_CODE(res), system_category());
#endif
  }

  void resize(console_size_t size)
  {
    error_code ec;
    resize(size, ec);
    if (ec)
      boost::process::v2::detail::throw_error(ec, "resize");
  }

#if defined(BOOST_PROCESS_V2_POSIX)
  error_code on_setup(posix::default_launcher & launcher, const filesystem::path &, const char * const *)
  {
    error_code ec;
    if (!is_open())
      this->open(console_size_t{80, 24}, ec);
    return ec;
  }

  error_code on_exec_setup(posix::default_launcher & launcher, const filesystem::path &, const char * const *)
  {
    error_code ec;
    auto nm = ptsname(handle_.native_handle());
    if (nm == nullptr)
      BOOST_PROCESS_V2_ASSIGN_EC(ec, ENODEV, system_category());
    else
    {
      auto s = ::open(nm, O_WRONLY);
      if (s < 0)
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

      auto t = ::open(nm, O_RDONLY);
      if (t < 0)
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

      if ((::dup2(t, STDIN_FILENO) == -1)
       || (::dup2(s, STDOUT_FILENO) == -1)
       || (::dup2(s, STDERR_FILENO) == -1)
       || (::setsid() == -1)
#ifdef TIOCSCTTY
       || ::ioctl(t, TIOCSCTTY, 0)
#endif
       || ::close(handle_.native_handle())
       || ::close(s)
       || ::close(t))
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
    }
    return error_code ();
  }
#else

  error_code on_setup(windows::default_launcher & launcher, const filesystem::path &, const std::wstring &)
  {
    error_code ec;
    if (!is_open())
      open(ec);

    auto &si = launcher.startup_info;

    size_t bytes_required;
    InitializeProcThreadAttributeList(NULL, 1, 0, &bytes_required);
    si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, bytes_required);
    if (!si.lpAttributeList)
    {
      BOOST_PROCESS_V2_ASSIGN_EC(ec, ENOMEM, system_category());
      return ec;
    }

    // Initialize the list memory location
    if (!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &bytes_required)
     || !UpdateProcThreadAttribute(si.lpAttributeList,
                                   0,
                                   PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                                   con_.get(),
                                   sizeof(con_.get()),
                                   NULL,
                                   NULL))
    {
      HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      return ec;
    }
  }
#endif
};

}

BOOST_PROCESS_V2_END_NAMESPACE

#endif // windows version
#endif //BOOST_PROCESS_VS_EXPERIMENTAL_BASIC_PTY_HPP
