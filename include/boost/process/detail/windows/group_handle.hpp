// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_WINDOWS_GROUP_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_GROUP_HPP_

#include <boost/process/detail/windows/handler.hpp>
#include <boost/detail/winapi/jobs.hpp>
#include <boost/process/detail/windows/child_handle.hpp>


namespace boost { namespace process { namespace detail { namespace windows {

void enable_break_away(::boost::detail::winapi::HANDLE_ h);
void enable_break_away(::boost::detail::winapi::HANDLE_ h, std::error_code & ec);

bool break_away_enabled(::boost::detail::winapi::HANDLE_ h);

struct group_handle
{
    ::boost::detail::winapi::HANDLE_ _job_object;

    typedef ::boost::detail::winapi::HANDLE_ handle_t;
    handle_t handle() const { return _job_object; }

    explicit group_handle(handle_t h) :
        _job_object(h)
    {
        enable_break_away(_job_object);
    }


    group_handle() : group_handle(::boost::detail::winapi::CreateJobObjectA(nullptr, nullptr))
    {

    }
    ~group_handle()
    {
        ::boost::detail::winapi::CloseHandle(_job_object);
    }
    group_handle(const group_handle & c) = delete;
    group_handle(group_handle && c) : _job_object(_job_object)
    {
        c._job_object = ::boost::detail::winapi::invalid_handle_value;
    }
    group_handle &operator=(const group_handle & c) = delete;
    group_handle &operator=(group_handle && c)
    {

        ::boost::detail::winapi::CloseHandle(_job_object);
        _job_object = c._job_object;
        c._job_object = ::boost::detail::winapi::invalid_handle_value;
        return *this;
    }

    void assign(handle_t proc)
    {
        if (!::boost::detail::winapi::AssignProcessToJobObject(_job_object, proc))
            throw_last_error();
    }
    void assign(handle_t proc, std::error_code & ec)
    {
        if (!::boost::detail::winapi::AssignProcessToJobObject(_job_object, proc))
            ec = get_last_error();
    }

    bool has(handle_t proc)
    {
        ::boost::detail::winapi::BOOL_ is;
        if (!::boost::detail::winapi::IsProcessInJob(proc, _job_object,  &is))
            throw_last_error();

        return is;
    }
    bool has(handle_t proc, std::error_code & ec)
    {
        ::boost::detail::winapi::BOOL_ is;
        if (!::boost::detail::winapi::IsProcessInJob(proc, _job_object,  &is))
            ec = get_last_error();
        return is;
    }

    bool valid() const
    {
        return _job_object != nullptr;
    }

};

inline void terminate(const group_handle &p)
{
    if (!::boost::detail::winapi::TerminateJobObject(p.handle(), EXIT_FAILURE))
        boost::process::detail::throw_last_error("TerminateJobObject() failed");
}

inline void terminate(const group_handle &p, std::error_code &ec)
{
    if (!::boost::detail::winapi::TerminateJobObject(p.handle(), EXIT_FAILURE))
        ec = boost::process::detail::get_last_error();
    else
        ec.clear();
}

inline bool in_group()
{
    ::boost::detail::winapi::BOOL_ res;
    if (!::boost::detail::winapi::IsProcessInJob(boost::detail::winapi::GetCurrentProcess(), nullptr, &res))
        throw_last_error("IsProcessInJob failed");

    return res;
}



}}}}


#endif /* BOOST_PROCESS_DETAIL_WINDOWS_GROUP_HPP_ */
