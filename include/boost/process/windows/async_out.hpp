// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_ASYNC_OUT_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_ASYNC_OUT_HPP

#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace boost { namespace process { namespace detail { namespace windows {

template<int p1, int p2>
struct async_out : public ::boost::process::detail::handler_base
{
    boost::iostreams::file_descriptor_source file;

    template<typename T>
    async_out(T&&) {}
    template <typename WindowsExecutor>
    void on_setup(WindowsExecutor &e) const;
};

template<>
template<typename WindowsExecutor>
void pipe_out<1,0>::on_setup<WindowsExecutor>(WindowsExecutor &e) const
{
}

template<>
template<typename WindowsExecutor>
void pipe_out<2,0>::on_setup<WindowsExecutor>(WindowsExecutor &e) const
{
}

template<>
template<typename WindowsExecutor>
void pipe_out<1,2>::on_setup<WindowsExecutor>(WindowsExecutor &e) const
{
}

}}}}

#endif
