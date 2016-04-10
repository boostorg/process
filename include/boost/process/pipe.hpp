// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/pipe.hpp
 *
 * Defines a pipe.
 */

#ifndef BOOST_PROCESS_PIPE_HPP
#define BOOST_PROCESS_PIPE_HPP

#include <boost/config.hpp>
#include <boost/process/detail/config.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/pipe.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/pipe.hpp>
#endif

namespace boost { namespace process {

struct pipe
{
    typedef boost::process::detail::api::pipe native_pipe;

    using sink_t   = boost::iostreams::file_descriptor_sink  ;
    using source_t = boost::iostreams::file_descriptor_source;
    typedef char                            char_type;

    typedef boost::iostreams::bidirectional_device_tag    category;

    pipe(boost::process::detail::api::pipe && handle)
        :  _native(std::move(handle))
    {
    }

    pipe() : pipe(boost::process::detail::api::pipe::create()) {}

    pipe(const std::string & name) : pipe(native_pipe::create_named(name)) {}

    pipe(      pipe&&) = default;
    pipe(const pipe& ) = delete;

    pipe& operator=(      pipe&&) = default;
    pipe& operator=(const pipe& ) = delete;

    std::streamsize read(char_type* s, std::streamsize n)
    {
        return _source.read(s, n);
    }
    std::streamsize write(const char_type* s, std::streamsize n)
    {
        return _sink.write(s, n);
    }
    const source_t & source() const {return _source;}
    const sink_t   & sink()   const {return _sink;}
    source_t & source() {return _source;}
    sink_t   & sink()   {return _sink;}

    bool is_open()
    {
        return _source.is_open() || _sink.is_open();
    }

    void close()
    {
        _source.close();
        _sink.close();
    }
private:
    native_pipe _native;
    boost::iostreams::file_descriptor_sink   _sink   {_native.sink(),   boost::iostreams::never_close_handle};
    boost::iostreams::file_descriptor_source _source {_native.source(), boost::iostreams::never_close_handle};

};

//using pipe_stream = boost::iostreams::stream<pipe>;

struct async_pipe : pipe
{
    typedef boost::process::detail::api::async_pipe_handle native_async_handle;
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


    native_async_handle &async_source() {return _source;}
    native_async_handle &async_sink  () {return _sink;}


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

    boost::asio::io_service& get_io_service() {return *_ios;}
    async_pipe(boost::asio::io_service & ios) :
            pipe(native_pipe::create_async()),
            _ios(&ios),
            _sink  (ios, pipe::sink().  handle()),
            _source(ios, pipe::source().handle())
    {}
    async_pipe(boost::asio::io_service & ios, const std::string & name) :
               pipe(name),
               _ios(&ios),
               _sink  (ios, pipe::sink().  handle()),
               _source(ios, pipe::source().handle())
       {}
    async_pipe(const async_pipe &) = default;
    async_pipe(async_pipe &&) = default;

    async_pipe& operator=(const async_pipe &) = default;
    async_pipe& operator=(async_pipe &&) = default;

private:
    boost::asio::io_service * _ios;
    native_async_handle _sink;
    native_async_handle _source;
};

}}



#endif
