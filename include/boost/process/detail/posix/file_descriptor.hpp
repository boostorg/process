// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_FILE_DESCRIPTOR_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_FILE_DESCRIPTOR_HPP_

#include <fcntl.h>
#include <string>
#include <boost/filesystem/path.hpp>

namespace boost { namespace process { namespace detail { namespace posix {

struct file_descriptor
{
    enum mode_t
    {
        read  = 1,
        write = 2,
        read_write = 3
    };
    static int desired_access(mode_t mode)
    {
        switch(mode)
        {
        case read:
            return O_RDONLY;
        case write:
            return O_WRONLY;
        case read_write:
            return O_RDWR;
        default:
            return 0u;
        }
    }

    file_descriptor() = default;
    file_descriptor(const boost::filesystem::path& p, mode_t mode = read_write)
        : file_descriptor(p.native(), mode)
    {
    }

    file_descriptor(const std::string & path , mode_t mode = read_write)
        : file_descriptor(path.c_str(), mode) {}


    file_descriptor(const char*    path, mode_t mode = read_write)
        : _handle(::open(path, desired_access(mode)))
    {

    }

    file_descriptor(const file_descriptor & ) = delete;
    file_descriptor(file_descriptor && ) = default;

    file_descriptor& operator=(const file_descriptor & ) = delete;
    file_descriptor& operator=(file_descriptor && ) = default;

    ~file_descriptor()
    {
        if (_handle != -1)
            ::close(_handle);
    }

    int handle() const { return _handle;}

private:
    int _handle = -1;
};

}}}}

#endif /* BOOST_PROCESS_DETAIL_WINDOWS_FILE_DESCRIPTOR_HPP_ */
