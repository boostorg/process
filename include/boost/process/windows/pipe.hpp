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
#include <boost/detail/winapi/pipes.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/file_management.hpp>
#include <boost/detail/winapi/get_last_error.hpp>
#include <boost/detail/winapi/access_rights.hpp>
#include <system_error>
#include <array>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace boost { namespace process { namespace detail { namespace windows {



class pipe
{
    boost::detail::winapi::HANDLE_ _source;
    boost::detail::winapi::HANDLE_ _sink;
protected:
    pipe(boost::detail::winapi::HANDLE_ source, boost::detail::winapi::HANDLE_ sink) : _source(source), _sink(sink) {}
public:
    pipe(const pipe& ) = delete;
    pipe(pipe&& lhs)   = default;
    pipe& operator=(const pipe& ) = delete;
    pipe& operator=(pipe&& lhs)   = default;

    void * source() const {return _source;}
    void * sink  () const {return _sink;}

    static pipe create()
    {
        boost::detail::winapi::HANDLE_ handles[2];
        if (!::boost::detail::winapi::CreatePipe(&handles[0], &handles[1], nullptr, 0))
            throw std::system_error(
                    std::error_code(
                    ::boost::detail::winapi::GetLastError(),
                    std::system_category()),
                    "CreatePipe() failed");
        return pipe(handles[0], handles[1]);
    }

    static pipe create(std::error_code &ec)
    {
        boost::detail::winapi::HANDLE_ handles[2];
        if (!::boost::detail::winapi::CreatePipe(&handles[0], &handles[1], nullptr, 0))
            ec = std::error_code(
                ::boost::detail::winapi::GetLastError(),
                std::system_category());
        else
            ec.clear();
        return pipe(handles[0], handles[1]);
    }
};
/*
struct async_pipe : pipe
{
    enum direction
    {
        none = 0b00,
        read = 0b01,
        write= 0b10,
        bidirectional = 0b11
    };
    static std::string get_pipe_name()
    {
        std::string name = "\\\\.\\pipe\\boost_process\\async_pipe_";

        boost::detail::winapi::WIN32_FIND_DATAA_ find_data;

        namespace fs = boost::filesystem;
        fs::path p = name + "0"; //first

        while (fs::exists(p)) //so it limits it to 2^31 pipes. should suffice.
        {
            static unsigned int i = 0;
            p = name + std::to_string(i);
            i++;
        }

        return p.string();

    }
    static async_pipe create(direction dir = bidirectional)
    {
        static constexpr int OPEN_EXISTING_         = 3; //temporary.
        static constexpr int FILE_FLAG_OVERLAPPED_  = 0x40000000; //temporary
        static constexpr int FILE_ATTRIBUTE_NORMAL_ = 0x00000080; //temporary

        boost::detail::winapi::DWORD_ pipe_flag = boost::detail::winapi::PIPE_ACCESS_INBOUND_;
        if (dir & write)
            pipe_flag = FILE_FLAG_OVERLAPPED_;

        std::string name = get_pipe_name();

        boost::detail::winapi::HANDLE_ handle1 = boost::detail::winapi::create_named_pipe(
                name.c_str(), pipe_flag, 0, 1, 8192, 8192, 0, nullptr);

        if (handle1 == boost::detail::winapi::INVALID_HANDLE_VALUE_)
            throw std::system_error(
                    std::error_code(
                    ::boost::detail::winapi::GetLastError(),
                    boost::system::system_category()),
                    "create_named_pipe() failed");

        boost::detail::winapi::DWORD_ file_flag = (dir & read) ?
                                                  FILE_FLAG_OVERLAPPED_ :
                                                  FILE_ATTRIBUTE_NORMAL_;

        boost::detail::winapi::HANDLE_ handle2 = boost::detail::winapi::create_file(
                name.c_str(),
                boost::detail::winapi::GENERIC_WRITE_, 0, nullptr,
                OPEN_EXISTING_,
                FILE_FLAG_OVERLAPPED_, nullptr);

        if (handle1 == boost::detail::winapi::INVALID_HANDLE_VALUE_)
            throw std::system_error(
                    std::error_code(
                    ::boost::detail::winapi::GetLastError(),
                    boost::system::system_category()),
                    "create_file() failed");

        return async_pipe(handle1, handle2);
    }

    static async_pipe create(std::error_code& ec) {return create(bidirectional, ec); }
    static async_pipe create(direction dir = bidirectional, std::error_code& ec)
    {
        static constexpr int OPEN_EXISTING_         = 3; //temporary.
        static constexpr int FILE_FLAG_OVERLAPPED_  = 0x40000000; //temporary
        static constexpr int FILE_ATTRIBUTE_NORMAL_ = 0x00000080; //temporary

        boost::detail::winapi::DWORD_ pipe_flag = boost::detail::winapi::PIPE_ACCESS_INBOUND_;
        if (dir & write)
            pipe_flag = FILE_FLAG_OVERLAPPED_;

        std::string name = get_pipe_name();

        boost::detail::winapi::HANDLE_ handle1 = boost::detail::winapi::create_named_pipe(
                name.c_str(), pipe_flag, 0, 1, 8192, 8192, 0, nullptr);

        if (handle1 == boost::detail::winapi::INVALID_HANDLE_VALUE_)
            ec = std::error_code(
                    ::boost::detail::winapi::GetLastError(),
                    std::system_category());

        boost::detail::winapi::DWORD_ file_flag = (dir & read) ?
                                                  FILE_FLAG_OVERLAPPED_ :
                                                  FILE_ATTRIBUTE_NORMAL_;

        boost::detail::winapi::HANDLE_ handle2 = boost::detail::winapi::create_file(
                name.c_str(),
                boost::detail::winapi::GENERIC_WRITE_, 0, nullptr,
                OPEN_EXISTING_,
                FILE_FLAG_OVERLAPPED_, nullptr);

        if (handle1 == boost::detail::winapi::INVALID_HANDLE_VALUE_)
            ec = std::error_code(
                    ::boost::detail::winapi::GetLastError(),
                    std::system_category());

        return async_pipe(handle1, handle2);
    }

private:
    using pipe::pipe;
};*/


}}}}

#endif
