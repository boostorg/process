//
// boost/process/v2/windows/impl/job_object_service.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Klemens D. Morgenstern (klemens dot morgenstern at gmx dot net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_PROCESS_V2_DETAIL_IMPL_GROUP_IMPL_WINDOWS_IPP
#define BOOST_PROCESS_V2_DETAIL_IMPL_GROUP_IMPL_WINDOWS_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/group_impl_windows.hpp>

#include <windows.h>

BOOST_PROCESS_V2_BEGIN_NAMESPACE
namespace detail
{

bool job_object_is_empty(HANDLE job_object, error_code & ec)
{
    JOBOBJECT_BASIC_ACCOUNTING_INFORMATION info;
    if (!QueryInformationJobObject(job_object,
            JobObjectBasicAccountingInformation,
            &info, sizeof(info), nullptr))
        ec = detail::get_last_error();
    return info.ActiveProcesses == 0u;
}



#if !defined(BOOST_PROCESS_V2_HEADER_ONLY)
template struct basic_group_impl<BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>;
#endif


void basic_group_impl_base::add(DWORD pid, HANDLE handle, error_code & ec)
{
    if (!AssignProcessToJobObject(job_object_.get(), handle))
        ec = detail::get_last_error();
}

void basic_group_impl_base::terminate(error_code & ec)
{
    if (!::TerminateJobObject(job_object_.get(), 255u))
        ec = detail::get_last_error();
}


bool basic_group_impl_base::contains(DWORD pid, error_code & ec)
{
    BOOL res = FALSE;
    // 
    struct del 
    {
        void operator()(HANDLE h)
        {
            if (h != nullptr && h != INVALID_HANDLE_VALUE)
                ::CloseHandle(h);
        }
    };

    std::unique_ptr<void, del> proc{::OpenProcess(PROCESS_QUERY_INFORMATION , FALSE, pid)};

    if (proc.get() != INVALID_HANDLE_VALUE &&        
        !IsProcessInJob(proc.get(), job_object_.get(), &res))
        ec = detail::get_last_error();

    return res == TRUE;
}

}
BOOST_PROCESS_V2_END_NAMESPACE


#endif //BOOST_PROCESS_V2_DETAIL_IMPL_GROUP_IMPL_WINDOWS_IPP