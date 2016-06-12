// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/process/posix.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>

using namespace boost::process;

int main()
{
//[bind_fd

    pipe p;
    system("test", posix::fd.bind(4, p.native_sink())    );
//]

//[close_fd
    system("test", posix::fd.close(STDIN_FILENO));
//]

//[close_fds
    system("test", posix::fd.close({STDIN_FILENO, STDOUT_FILENO}));
//]

//[fork_execve
    const char *env[2] = { 0 };
    env[0] = "LANG=de";
    system("test",
        on_setup([env](auto &e) { e.env = const_cast<char**>(env); }),
        posix::on_fork_error([](auto&)
            { std::cerr << errno << std::endl; }),
        posix::on_exec_setup([](auto&)
            { ::chroot("/new/root/directory/"); }),
        posix::on_exec_error([](auto&)
            { std::ofstream ofs("log.txt"); if (ofs) ofs << errno; })
    );
//]
}
