// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_PIPE_HPP
#define BOOST_PROCESS_WINDOWS_PIPE_HPP

#include <boost/detail/winapi/basic_types.hpp>

namespace boost { namespace process { namespace windows {

struct pipe
{
    boost::detail::winapi::HANDLE_ source;
    boost::detail::winapi::HANDLE_ sink;

    pipe(boost::detail::winapi::HANDLE_ source, boost::detail::winapi::HANDLE_ sink) : source(source), sink(sink) {}
};

inline pipe make_pipe(boost::detail::winapi::HANDLE_ source,
					  boost::detail::winapi::HANDLE_ sink)
{
    return pipe(source, sink);
}

}}}

#endif
