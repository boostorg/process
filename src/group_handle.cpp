
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/windows/group_ref.hpp>
#include <windows.h>

#include <iostream>

namespace boost { namespace process { namespace detail { namespace windows {



bool break_away_enabled(HANDLE h)
{
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;

    if (!QueryInformationJobObject(
                    h,
                    JobObjectExtendedLimitInformation,
                    static_cast<void*>(&info),
                    sizeof(info),
                    NULL))
        throw_last_error("QueryInformationJobObject() failed");

    return (info.BasicLimitInformation.LimitFlags & JOB_OBJECT_LIMIT_BREAKAWAY_OK) != 0;
}

void enable_break_away(HANDLE h)
{
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;

    if (!QueryInformationJobObject(
                    h,
                    JobObjectExtendedLimitInformation,
                    static_cast<void*>(&info),
                    sizeof(info),
                    NULL))
        throw_last_error("QueryInformationJobObject() failed");

    if ((info.BasicLimitInformation.LimitFlags & JOB_OBJECT_LIMIT_BREAKAWAY_OK) != 0)
        return;

    info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_BREAKAWAY_OK;

    if (!SetInformationJobObject(
                h,
                JobObjectExtendedLimitInformation,
                static_cast<void*>(&info),
                sizeof(info)))
        throw_last_error("SetInformationJobObject() failed");
}

void enable_break_away(HANDLE h, std::error_code & ec)
{
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;


    if (!QueryInformationJobObject(
                    h,
                    JobObjectExtendedLimitInformation,
                    static_cast<void*>(&info),
                    sizeof(info),
                    NULL))
    {
        ec = get_last_error();
        return;
    }

    info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_BREAKAWAY_OK;

    if (!SetInformationJobObject(
                h,
                JobObjectExtendedLimitInformation,
                static_cast<void*>(&info),
                sizeof(info)))
    {
        ec = get_last_error();
        return;
    }}


}}}}



