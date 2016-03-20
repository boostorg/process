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
#include <boost/process/config.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix/pipe.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/windows/pipe.hpp>
#endif

namespace boost { namespace process {

class pipe
{

    boost::iostreams::file_descriptor_sink   _sink  ;
    boost::iostreams::file_descriptor_source _source;
public:

    using sink_t   = boost::iostreams::file_descriptor_sink  ;
    using source_t = boost::iostreams::file_descriptor_source;
    typedef char                            char_type;

    typedef boost::iostreams::bidirectional_device_tag    category;
    typedef boost::process::detail::api::pipe native_pipe;

    pipe(boost::process::detail::api::pipe && handle)
        :  _sink  (handle.sink(),   boost::iostreams::close_handle),
           _source(handle.source(), boost::iostreams::close_handle)
    {}

    pipe() : pipe(boost::process::detail::api::pipe::create()) {}

    pipe(      pipe&&) = default;
    pipe(const pipe& ) = default;

    pipe& operator=(      pipe&&) = default;
    pipe& operator=(const pipe& ) = default;

    static pipe create()                    { return pipe(native_pipe::create());  }
    static pipe create(std::error_code &ec) { return pipe(native_pipe::create(ec));}

    static pipe create(const std::string & name)                      { return create_named(name);  }
    static pipe create(const std::string & name, std::error_code &ec) { return create_named(name, ec);}


    inline static std::string make_pipe_name() {return native_pipe::make_pipe_name(); }

    inline static pipe create_named(const std::string & name = make_pipe_name())    {return native_pipe::create_named(name);}
    inline static pipe create_named(const std::string & name, std::error_code & ec) {return native_pipe::create_named(name,ec);}


    inline static pipe create_named(std::error_code & ec) {return create_named(make_pipe_name(), ec);}

    inline static pipe create_async()                     {return native_pipe::create_async(); }
    inline static pipe create_async(std::error_code & ec) {return native_pipe::create_async(ec); }




    std::streamsize read(char_type* s, std::streamsize n)
    {
        return _source.read(s, n);
    }
    std::streamsize write(const char_type* s, std::streamsize n)
    {
        return _sink.write(s, n);
    }


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

};

using pipe_stream = boost::iostreams::stream<pipe>;

}}



#endif
