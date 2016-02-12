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


namespace boost { namespace process {

#if defined(BOOST_POSIX_API)
using boost::process::posix::pipe;
#elif defined(BOOST_WINDOWS_API)
using boost::process::windows::pipe;
#endif

}}

#if defined(BOOST_PROCESS_DOXYGEN)
namespace boost { namespace process {

/**
 * Represents a pipe.
 */
struct pipe
{
    /**
     * Read-end.
     */
    pipe_end_type source;

    /**
     * Write-end.
     */
    pipe_end_type sink;

    /**
     * Default constructor, creates a pipe.
     */
    pipe();

    /**
     * Copy constructor.
     */
    pipe(const pipe& p);

    /**
     * Create a pipe.
     */
    static pipe create();

};


}}
#endif

#endif
