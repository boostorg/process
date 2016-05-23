// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_WINDOWS_ASYNC_PIPE_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_ASYNC_PIPE_HPP_

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/pipes.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/file_management.hpp>
#include <boost/detail/winapi/get_last_error.hpp>
#include <boost/detail/winapi/access_rights.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/process/detail/windows/basic_pipe.hpp>
#include <boost/asio/windows/stream_handle.hpp>
#include <system_error>
#include <string>

namespace boost { namespace process { namespace detail { namespace windows {

inline std::string make_pipe_name()
{
    std::string name = "\\\\.\\pipe\\boost_process_auto_pipe_";

    auto pid = ::boost::detail::winapi::GetCurrentProcessId();

    static unsigned long long cnt = 0;
    name += pid;
    name += "_";
    name += cnt;

    return name;
}

class async_pipe
{
    ::boost::asio::windows::stream_handle _source;
    ::boost::asio::windows::stream_handle _sink  ;
public:
    inline async_pipe(boost::asio::io_service & ios, const std::string & name = make_pipe_name());

    inline explicit async_pipe(const std::string & name);
    inline async_pipe(const basic_pipe& p);
    async_pipe(async_pipe&& lhs)  : _source(std::move(lhs._source)), _sink(std::move(lhs._sink))
    {
        lhs._source.assign (::boost::detail::winapi::INVALID_HANDLE_VALUE_);
        lhs._sink  .assign (::boost::detail::winapi::INVALID_HANDLE_VALUE_);
    }
    template<class CharT, class Traits = std::char_traits<CharT>>
    explicit async_pipe(boost::asio::io_service & ios, const pipe<CharT, Traits> & p) : _source(ios, p.source()), _sink(ios, p.sink())
    {
    }

    inline async_pipe& operator=(const basic_pipe& p);
    async_pipe& operator=(async_pipe&& lhs)
    {
        _source = std::move(lhs._source);
        _sink   = std::move(lhs._sink);
        lhs._source .assign(::boost::detail::winapi::INVALID_HANDLE_VALUE_);
        lhs._sink   .assign(::boost::detail::winapi::INVALID_HANDLE_VALUE_);
        return *this;
    }
    ~async_pipe()
    {
        if (_sink   != ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
            ::boost::detail::winapi::CloseHandle(_sink.native());
        if (_source != ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
            ::boost::detail::winapi::CloseHandle(_source.native());
    }

    template<class CharT, class Traits = std::char_traits<CharT>>
    inline explicit operator basic_pipe<CharT, Traits>() const;

    void cancel()
    {
        if (_sink.is_open())
            _sink.cancel();
        if (_source.is_open())
            _source.cancel();
    }

    void close()
    {
        if (_sink.is_open())
            _sink.cancel();
        if (_source.is_open())
            _source.cancel();}

    bool is_open() const
    {
        return  _sink.is_open() || _source.is_open();
    }
    void async_close()
    {
        if (_sink.is_open())
            _sink.get_io_service().  post([this]{_sink.close();});
        if (_source.is_open())
            _source.get_io_service().post([this]{_source.close();});
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence & buffers)
    {
        return _source.read_some(buffers);
    }
    template<typename MutableBufferSequence>
    std::size_t write_some(const MutableBufferSequence & buffers)
    {
        return _sink.write_some(buffers);
    }


    void* source() const {return _source.native();}
    void* sink  () const {return _sink  .native();}


    template<typename MutableBufferSequence,
             typename ReadHandler>
    void async_read_some(
        const MutableBufferSequence & buffers,
              ReadHandler handler)
    {
        _source.async_read_some(buffers, handler);
    }

    template<typename ConstBufferSequence,
             typename WriteHandler>
    void async_write_some(
        const ConstBufferSequence & buffers,
        WriteHandler handler)
    {
        _sink.async_write_some(buffers, handler);
    }

};


async_pipe::async_pipe(boost::asio::io_service & ios, const std::string & name) : _source(ios), _sink(ios)
{
    static constexpr int OPEN_EXISTING_         = 3; //temporary.
    static constexpr int FILE_FLAG_OVERLAPPED_  = 0x40000000; //temporary
    //static constexpr int FILE_ATTRIBUTE_NORMAL_ = 0x00000080; //temporary

    ::boost::detail::winapi::HANDLE_ source = ::boost::detail::winapi::create_named_pipe(
            name.c_str(),
            ::boost::detail::winapi::PIPE_ACCESS_INBOUND_
            | FILE_FLAG_OVERLAPPED_, //write flag
            0, 1, 8192, 8192, 0, nullptr);

    if (source == boost::detail::winapi::INVALID_HANDLE_VALUE_)
        ::boost::process::detail::throw_last_error("create_named_pipe() failed");

    _source.assign(source);

    ::boost::detail::winapi::HANDLE_ sink = boost::detail::winapi::create_file(
            name.c_str(),
            ::boost::detail::winapi::GENERIC_WRITE_, 0, nullptr,
            OPEN_EXISTING_,
            FILE_FLAG_OVERLAPPED_, //to allow read
            nullptr);

    if (sink == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        ::boost::process::detail::throw_last_error("create_file() failed");

    _sink.assign(sink);
}

async_pipe& async_pipe::operator=(const async_pipe & p)
{
    auto proc = ::boost::detail::winapi::GetCurrentProcess();

    ::boost::detail::winapi::HANDLE_ source;
    ::boost::detail::winapi::HANDLE_ sink;

    //cannot get the handle from a const object.
    auto source_in = const_cast<::boost::asio::windows::stream_handle &>(p._source).native();
    auto sink_in   = const_cast<::boost::asio::windows::stream_handle &>(p._sink).native();

    if (source_in == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        source = ::boost::detail::winapi::INVALID_HANDLE_VALUE_;
    else if (!::boost::detail::winapi::DuplicateHandle(
            proc, source_in, proc, &source, 0,
            static_cast<::boost::detail::winapi::BOOL_>(true),
             ::boost::detail::winapi::DUPLICATE_SAME_ACCESS_))
        throw_last_error("Duplicate Pipe Failed");

    if (sink_in   == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        sink = ::boost::detail::winapi::INVALID_HANDLE_VALUE_;
    else if (!::boost::detail::winapi::DuplicateHandle(
            proc, sink_in, proc, &sink, 0,
            static_cast<::boost::detail::winapi::BOOL_>(true),
             ::boost::detail::winapi::DUPLICATE_SAME_ACCESS_))
        throw_last_error("Duplicate Pipe Failed");

    _source.assign(source);
    _sink.  assign(sink);

    return *this;
}

async_pipe& async_pipe::operator=(async_pipe && lhs)
{
    if (_source == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        ::boost::detail::winapi::CloseHandle(_source.native());

    if (_sink == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        ::boost::detail::winapi::CloseHandle(_sink.native());

    _source = lhs._source;
    _sink   = lhs._sink;
    lhs._source = ::boost::detail::winapi::INVALID_HANDLE_VALUE_;
    lhs._sink   = ::boost::detail::winapi::INVALID_HANDLE_VALUE_;
    return *this;
}

template<class CharT, class Traits = std::char_traits<CharT>>
async_pipe::operator basic_pipe<CharT, Traits>() const
{
    auto proc = ::boost::detail::winapi::GetCurrentProcess();

    ::boost::detail::winapi::HANDLE_ source;
    ::boost::detail::winapi::HANDLE_ sink;

    //cannot get the handle from a const object.
    auto source_in = const_cast<::boost::asio::windows::stream_handle &>(_source).native();
    auto sink_in   = const_cast<::boost::asio::windows::stream_handle &>(_sink).native();

    if (source == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        _source = ::boost::detail::winapi::INVALID_HANDLE_VALUE_;
    else if (!::boost::detail::winapi::DuplicateHandle(
            proc, source_in, proc, &source, 0,
            static_cast<::boost::detail::winapi::BOOL_>(true),
             ::boost::detail::winapi::DUPLICATE_SAME_ACCESS_))
        throw_last_error("Duplicate Pipe Failed");

    if (sink == ::boost::detail::winapi::INVALID_HANDLE_VALUE_)
        _sink = ::boost::detail::winapi::INVALID_HANDLE_VALUE_;
    else if (!::boost::detail::winapi::DuplicateHandle(
            proc, sink_in, proc, &sink, 0,
            static_cast<::boost::detail::winapi::BOOL_>(true),
             ::boost::detail::winapi::DUPLICATE_SAME_ACCESS_))
        throw_last_error("Duplicate Pipe Failed");

    return {source, sink};
}

inline bool operator==(const async_pipe & lhs, const async_pipe & rhs)
{
    return compare_handles(lhs.source(), rhs.source()) &&
           compare_handles(lhs.sink(),   rhs.sink());
}

inline bool operator!=(const async_pipe & lhs, const async_pipe & rhs)
{
    return !compare_handles(lhs.source(), rhs.source()) ||
           !compare_handles(lhs.sink(),   rhs.sink());
}

template<class Char, class Traits>
inline bool operator==(const async_pipe & lhs, const basic_pipe<Char, Traits> & rhs)
{
    return compare_handles(lhs.source(), rhs.source()) &&
           compare_handles(lhs.sink(),   rhs.sink());
}

template<class Char, class Traits>
inline bool operator!=(const async_pipe & lhs, const basic_pipe<Char, Traits> & rhs)
{
    return !compare_handles(lhs.source(), rhs.source()) ||
           !compare_handles(lhs.sink(),   rhs.sink());
}

template<class Char, class Traits>
inline bool operator==(const basic_pipe<Char, Traits> & lhs, const async_pipe & rhs)
{
    return compare_handles(lhs.source(), rhs.source()) &&
           compare_handles(lhs.sink(),   rhs.sink());
}

template<class Char, class Traits>
inline bool operator!=(const basic_pipe<Char, Traits> & lhs, const async_pipe & rhs)
{
    return !compare_handles(lhs.source(), rhs.source()) ||
           !compare_handles(lhs.sink(),   rhs.sink());
}

}}}}

#endif /* INCLUDE_BOOST_PIPE_DETAIL_WINDOWS_ASYNC_PIPE_HPP_ */
