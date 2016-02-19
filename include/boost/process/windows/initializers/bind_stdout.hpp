// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_BIND_STDOUT_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_BIND_STDOUT_HPP

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/detail/winapi/handle_info.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/process/detail/initializers/handler_base.hpp>

namespace boost { namespace process { namespace windows { namespace initializers {

struct bind_stdout : ::boost::process::detail::initializers::handler_base
{
public:
    explicit bind_stdout(const boost::iostreams::file_descriptor_sink &sink) : sink_(sink) {}

    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        ::boost::detail::winapi::SetHandleInformation(
        		sink_.handle(),
        		::boost::detail::winapi::HANDLE_FLAG_INHERIT_,
				::boost::detail::winapi::HANDLE_FLAG_INHERIT_);
        e.startup_info.hStdOutput = sink_.handle();
        e.startup_info.dwFlags |= ::boost::detail::winapi::STARTF_USESTDHANDLES_;
        e.inherit_handles = true;
    }

private:
    boost::iostreams::file_descriptor_sink sink_;
};

}}}}

#endif
