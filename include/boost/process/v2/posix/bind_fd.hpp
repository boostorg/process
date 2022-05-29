// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_POSIX_BIND_FD_HPP
#define BOOST_PROCESS_V2_POSIX_BIND_FD_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/default_launcher.hpp>

#include <fcntl.h>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace posix
{

struct bind_fd
{
    int target;
    int fd;
    bool fd_needs_closing{false};

    ~bind_fd()
    {
        if (fd_needs_closing)
            ::close(fd);
    }

    bind_fd() = delete;
    bind_fd(int target) : target(target), fd(target) {}
    template<typename Stream>
    bind_fd(Stream && str, decltype(std::declval<Stream>().native_handle()) = -1)
        : bind_fd(str.native_handle())
    {}
    bind_fd(int target, FILE * f) : bind_fd(target, fileno(f)) {}
    bind_fd(int target, int fd) : target(target), fd(fd) {}
    bind_fd(int target, std::nullptr_t) : bind_fd(target, filesystem::path("/dev/null")) {}
    bind_fd(int target, const filesystem::path & pth, int flags = O_RDWR | O_CREAT)
            : target(target), fd(::open(pth.c_str(), flags, 0660)), fd_needs_closing(true)
    {
    }

    error_code on_exec_setup(posix::default_launcher & launcher, const filesystem::path &, const char * const *)
    {
        if (::dup2(fd, target) == -1)
            return error_code(errno, system_category());
        else
            return error_code ();
    }
};

}

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_POSIX_BIND_FD_HPP
