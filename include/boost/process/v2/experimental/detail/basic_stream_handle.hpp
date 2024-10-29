//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_V2_EXPERIMENTAL_DETAIL_BASIC_STREAM_HANDLE_HPP
#define BOOST_PROCESS_V2_EXPERIMENTAL_DETAIL_BASIC_STREAM_HANDLE_HPP

#include <boost/process/v2/detail/config.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)

#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/experimental/console_size.hpp>

#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/append.hpp>
#include <asio/compose.hpp>
#include <asio/dispatch.hpp>
#include <asio/steady_timer.hpp>
#include <asio/windows/basic_object_handle.hpp>
#include <asio/windows/basic_stream_handle.hpp>
#else
#include <boost/asio/append.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/windows/basic_object_handle.hpp>
#include <boost/asio/windows/basic_stream_handle.hpp>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE
namespace detail
{
namespace experimental
{

template<typename Executor>
struct basic_stream_handle
{
  net::windows::basic_object_handle<Executor> object;
  net::windows::basic_stream_handle<Executor> stream;
  v2::experimental::console_size_t cs_buf_{0u, 0u};

  template<typename Arg>
  basic_stream_handle(Arg &&arg) : object{arg}, stream{arg} {}

  template<typename Executor1>
  basic_stream_handle(basic_stream_handle<Executor1> &&lhs)
      : object(std::move(lhs.object)), stream(std::move(lhs.stream)), cs_buf_(lhs.cs_buf_) {}

  using executor_type = Executor;
  executor_type get_executor() noexcept { return object.get_executor(); }


  void assign(HANDLE h, error_code &ec)
  {
    assert(GetFileType(h) == FILE_TYPE_CHAR);
    if (GetFileType(h) == FILE_TYPE_CHAR)
    {
      stream.close(ec);
      object.assign(h, ec);
      DWORD flags;
      if (!::GetConsoleMode(h, &flags)) {
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
        return;
      }

      if (!::SetConsoleMode(h, flags | ENABLE_VIRTUAL_TERMINAL_INPUT)) // probably output!
       if (!::SetConsoleMode(h, flags | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING  ))
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      if (!::SetConsoleCP(CP_UTF8))
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      if (!::SetConsoleOutputCP(CP_UTF8))
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);


      CONSOLE_SCREEN_BUFFER_INFO bi;
      if (::GetConsoleScreenBufferInfo(h, &bi))
      {
        cs_buf_.columns = bi.dwSize.X;
        cs_buf_.rows = bi.dwSize.Y;
      }

    }
    else
    {
      object.close(ec);
      stream.assign(h, ec);
    }
  }

  void assign(HANDLE h)
  {
    error_code ec;
    assign(h, ec);
    if (ec)
      detail::throw_error(ec, "assign");
  }

  void close(error_code &ec)
  {
    object.close(ec);
    stream.close(ec);
  }

  void close()
  {
    object.close();
    stream.close();
  }

  HANDLE native_handle()
  {
    return object.is_open() ? object.native_handle() : stream.native_handle();
  }

  HANDLE release()
  {
    return object.is_open() ? object.release() : stream.release();
  }

  void cancel(error_code &ec)
  {
    if (object.is_open())
      object.cancel(ec);
    else if (stream.is_open())
      stream.cancel(ec);

    trigger_size_.cancel(ec);
  }

  void cancel()
  {
    if (object.is_open())
      object.cancel();
    else if (stream.is_open())
      stream.cancel();

    trigger_size_.cancel();
  }

  bool is_open() const { return object.is_open() || stream.is_open(); }

  void wait(error_code &ec)
  {
    if (object.is_open())
      object.wait(ec);
    else
      BOOST_PROCESS_V2_ASSIGN_EC(ec, net::error::operation_not_supported);
  }

  void wait()
  {
    error_code ec;
    wait(ec);
    if (ec)
      detail::throw_error(ec);
  }

  struct async_wait_op
  {
    template<typename Handler>
    void operator()(Handler &&handler, basic_stream_handle *this_)
    {
      if (this_->object.is_open())
        this_->object.async_wait(std::move(handler));
      else {
        auto e = net::get_associated_immediate_executor(handler, this_->stream.get_executor());
        net::dispatch(e, net::append(std::move(handler), net::error::operation_not_supported));
      }
    }
  };

  template<typename Token>
  auto async_wait(Token &&token)
  {
    return net::async_initiate<Token, void(error_code)>(async_wait_op{}, token, this);
  }

  template<typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence &mbs, error_code &ec)
  {
    if (stream.is_open())
      return stream.read_some(mbs, ec);


    asio::mutable_buffer buf;

    for (auto itr = net::buffer_sequence_begin(mbs);
         itr != net::buffer_sequence_end(mbs); itr++)
      if (itr->size() > 0)
      {
        buf = *itr;
        break;
      }

    if (buf.size() == 0u)
      return 0u;

    DWORD read_size = 0u;
    if (!::ReadFile(object.native_handle(), buf.data(), buf.size(), &read_size, NULL))
    {
      const DWORD last_error = ::GetLastError();
      if (ERROR_END_OF_MEDIA == last_error)
        BOOST_PROCESS_V2_ASSIGN_EC(ec, net::error::eof);
      else
        BOOST_PROCESS_V2_ASSIGN_EC(ec, last_error, system_category());
    }

    return static_cast<std::size_t>(read_size);
  }

  template<typename MutableBuffer>
  struct async_read_some_op
  {
    basic_stream_handle *this_;
    MutableBuffer buffer;

    template<typename Self>
    void operator()(Self &&self)
    {
      if (this_->stream.is_open())
        return this_->stream.async_read_some(buffer, std::move(self));

      this_->async_wait(std::move(self));
    }

    template<typename Self>
    void operator()(Self &&self, error_code ec)
    {
      asio::mutable_buffer buf;

      for (auto itr = net::buffer_sequence_begin(buffer);
           itr != net::buffer_sequence_end(buffer); itr++)
        if (itr->size() > 0)
        {
          buf = *itr;
          break;
        }

      if (buf.size() == 0u)
        return self.complete(error_code{}, 0u);

      INPUT_RECORD in_buffer[8092];
      DWORD read_size = 0u;
      if (!::PeekConsoleInputW(this_->object.native_handle(), in_buffer, 8092, &read_size))
      {
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
        return self.complete(ec, 0u);
      }
      auto begin = in_buffer,
             end = in_buffer + read_size;

      std::size_t keys = 0u;
      bool has_cr = false;
      for (auto itr = begin; itr != end; itr++)
      {
        if (itr->EventType == KEY_EVENT)
        {
          keys += itr->Event.KeyEvent.wRepeatCount;
          has_cr |= itr->Event.KeyEvent.uChar.AsciiChar == '\r';
        }
      }

      if (keys == 0u)
      {
        if (!::FlushConsoleInputBuffer(this_->object.native_handle()))
        {
          BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
          return self.complete(ec, 0u);
        }
        else
          return this_->async_wait(std::move(self));
      }

      if (!has_cr)
      {
        DWORD mode = 0;
        if (!::GetConsoleMode(this_->object.native_handle(), &mode))
          BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
        else if ((mode & ENABLE_LINE_INPUT) != 0)
          BOOST_PROCESS_V2_ASSIGN_EC(ec, net::error::would_block); // line mode would block on windows
      }
      if (ec)
        return self.complete(ec, 0u);

      DWORD to_read = (std::min)(keys, buf.size());

      read_size = 0u;
      if (!ReadFile(this_->object.native_handle(), buf.data(), to_read, &read_size, NULL))
      {
        const DWORD last_error = ::GetLastError();
        if (ERROR_END_OF_MEDIA == last_error)
          BOOST_PROCESS_V2_ASSIGN_EC(ec, net::error::eof);
        else
          BOOST_PROCESS_V2_ASSIGN_EC(ec, last_error, system_category());
      }

      self.complete(ec, static_cast<std::size_t>(read_size));
    }

    template<typename Self>
    void operator()(Self &&self, error_code ec, std::size_t n_)
    {
      self.complete(ec, n_);
    }

  };

  template<typename MutableBuffer, typename CompletionToken>
  auto async_read_some(const MutableBuffer &buffer, CompletionToken &&token)
    -> decltype(net::async_initiate<CompletionToken,
        void(boost::system::error_code, std::size_t)>(
        net::composed(async_read_some_op<MutableBuffer>{this, buffer}, stream), token))
  {
    return net::async_initiate<CompletionToken,
        void(boost::system::error_code, std::size_t)>(
        net::composed(async_read_some_op<MutableBuffer>{this, buffer}, stream), token);
  }

  template<typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence &cbs, error_code &ec)
  {
    if (stream.is_open())
      return stream.write_some(cbs, ec);

    net::const_buffer buf;
    for (auto itr = net::buffer_sequence_begin(cbs);
         itr != net::buffer_sequence_end(cbs); itr++)
      if (itr->size() > 0)
      {
        buf = *itr;
        break;
      }

    if (buf.size() == 0u)
      return 0u;

    // get one screen size, that's how much we'll write
    CONSOLE_SCREEN_BUFFER_INFO bi;
    if (!GetConsoleScreenBufferInfo(object.native_handle(), &bi)) {
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);
      return 0u;
    }

    const auto sz = (std::min)(static_cast<std::size_t>(bi.dwSize.X * bi.dwSize.Y), buf.size());

    DWORD wr = 0u;
    if (!WriteConsoleA(object.native_handle(), buf.data(), sz, &wr, NULL))
      BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec);

    return static_cast<std::size_t>(wr);
  }

  template<typename ConstBufferSequence>
  struct async_write_op
  {
    basic_stream_handle *this_;
    ConstBufferSequence buffer;

    template<typename Handler>
    void operator()(Handler &&handler)
    {
      if (this_->stream.is_open())
        return this_->stream.async_write_some(buffer, std::move(handler));

      error_code ec;
      auto n = this_->write_some(buffer, ec);
      auto exec = net::get_associated_immediate_executor(
          handler, this_->object.get_executor());
      net::dispatch(exec, net::append(std::move(handler), ec, n));

    }
  };

  template<typename ConstBufferSequence, typename CompletionToken>
  auto async_write_some(const ConstBufferSequence & buffer, CompletionToken && token)
    -> decltype(net::async_initiate<CompletionToken,
        void(boost::system::error_code, std::size_t)>(
        async_write_op<ConstBufferSequence>{this, buffer}, token))
  {
    return net::async_initiate<CompletionToken,
        void(boost::system::error_code, std::size_t)>(
        async_write_op<ConstBufferSequence>{this, buffer}, token);
  }

};
}

}
BOOST_PROCESS_V2_END_NAMESPACE

#endif
#endif //BOOST_PROCESS_V2_EXPERIMENTAL_DETAIL_BASIC_STREAM_HANDLE_HPP
