// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_START_DIR_HPP
#define BOOST_PROCESS_DETAIL_POSIX_START_DIR_HPP

#include <boost/process/detail/posix/handler.hpp>
#include <string>
#include <unistd.h>

namespace boost { namespace process { namespace detail { namespace posix {


struct start_dir_init : handler_base_ext
{
public:
    explicit start_dir_init(const std::string &s) : s_(s) {}

    template <class PosixExecutor>
    void on_exec_setup(PosixExecutor&) const
    {
        ::chdir(s_.c_str());
    }

private:
    std::string s_;
};

}}}}

#endif
