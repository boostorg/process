// Copyright (c) 2019 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_HANDLES_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_HANDLES_HPP_

#include <vector>
#include <system_error>
#include <dirent.h>

namespace boost { namespace process { namespace detail { namespace posix {


using native_handle_type = int;

inline std::vector<native_handle_type> get_handles(std::error_code & ec)
{
    std::vector<native_handle_type> res;

    auto dir = ::opendir("/dev/fd");
    auto my_fd = ::dirfd(dir);
    if (!dir)
    {
        ec = ::boost::process::detail::get_last_error();
    }
    else
        ec.clear();

    struct ::dirent * ent_p;

    while ((ent_p = readdir(dir)) != nullptr)
    {
        if (ent_p->d_name[0] == '.')
            continue;

        const auto conv = std::atoi(ent_p->d_name);
        if (conv == 0 && (ent_p->d_name[0] != '0' && ent_p->d_name[1] != '\0'))
            continue;

        if (conv == my_fd)
            continue;

        res.push_back(conv);
    }

    return res;
}

inline std::vector<native_handle_type> get_handles()
{
    std::error_code ec;

    auto res = get_handles(ec);
    if (ec)
        boost::process::detail::throw_error(ec, "open_dir(\"/dev/fd\") failed");

    return res;
}


inline bool is_stream_handle(native_handle_type handle, std::error_code & ec)
{
    struct ::stat stat_;

    if (::fstat(handle, &stat_) != 0)
    {
        ec = ::boost::process::detail::get_last_error();
    }
    else
        ec.clear();

    return S_ISCHR  (stat_.st_mode)  //This macro returns non-zero if the file is a character special file (a device like a terminal).
        || S_ISBLK  (stat_.st_mode) // This macro returns non-zero if the file is a block special file (a device like a disk).
        || S_ISREG  (stat_.st_mode) // This macro returns non-zero if the file is a regular file.
        || S_ISFIFO (stat_.st_mode) // This macro returns non-zero if the file is a FIFO special file, or a pipe. See section 15. Pipes and FIFOs.
        || S_ISSOCK (stat_.st_mode) ;// This macro returns non-zero if the file is a socket. See section 16. Sockets.;
}


inline bool is_stream_handle(native_handle_type handle)
{
    std::error_code ec;
    auto res = is_stream_handle(handle, ec);
    if (ec)
        boost::process::detail::throw_error(ec, "fstat() failed");

    return res;
}

}}}}

#endif //PROCESS_HANDLES_HPP
