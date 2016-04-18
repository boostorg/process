// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_FILE_OUT_HPP
#define BOOST_PROCESS_POSIX_FILE_OUT_HPP

#include <boost/process/detail/posix/handler.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include <unistd.h>
namespace boost { namespace process { namespace detail { namespace posix {

template<int p1, int p2>
struct file_out : handler_base_ext
{
    boost::iostreams::file_descriptor_sink file;

    template<typename T>
    file_out(T&& t) : file(std::forward<T>(t)) {}
    file_out(FILE * f) : file(fileno(f), boost::iostreams::never_close_handle) {}
    file_out(const boost::iostreams::file_descriptor_sink &f) : file(f.handle(), boost::iostreams::never_close_handle) {}


    template <typename Executor>
    void on_exec_setup(Executor &e) const;
};

template<>
template<typename Executor>
void file_out<1,-1>::on_exec_setup(Executor &e) const
{
    ::dup2(file.handle(), STDOUT_FILENO);
}

template<>
template<typename Executor>
void file_out<2,-1>::on_exec_setup(Executor &e) const
{
    ::dup2(file.handle(), STDERR_FILENO);
}

template<>
template<typename Executor>
void file_out<1,2>::on_exec_setup(Executor &e) const
{
    
    ::dup2(file.handle(), STDOUT_FILENO);
    ::dup2(file.handle(), STDERR_FILENO);

}

}}}}

#endif
