// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_PIPE_IN_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_PIPE_IN_HPP

#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/process/pipe.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <cstdio>
#include <io.h>

namespace boost { namespace process { namespace detail { namespace windows {

struct pipe_in : public ::boost::process::detail::handler_base
{
    boost::iostreams::file_descriptor_source file;
    pipe_in(FILE * f) : file(_get_osfhandle(_fileno(f)), boost::iostreams::never_close_handle) {}
    pipe_in(const boost::process::pipe & p) : file(p.source().handle(), boost::iostreams::never_close_handle) {}
    pipe_in(const boost::iostreams::file_descriptor_source &f) : file(f) {}

    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.startup_info.hStdInput = file.handle();
        e.startup_info.dwFlags  |= boost::detail::winapi::STARTF_USESTDHANDLES_;
    }
};

}}}}

#endif
