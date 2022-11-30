// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_DETAIL_EXE_IPP
#define BOOST_PROCESS_V2_IMPL_DETAIL_EXE_IPP

#include <boost/process/v2/detail/exe.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

namespace ext
{

#if defined(_WIN32)

BOOST_PROCESS_V2_DECL
HANDLE open_process_with_debug_privilege(pid_type pid) {
    HANDLE proc = nullptr;
    HANDLE hToken = nullptr;
    LUID luid;
    TOKEN_PRIVILEGES tkp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if (LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid)) {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Luid = luid;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            if (AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), nullptr, nullptr)) {
                proc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
            }
        }
        CloseHandle(hToken);
    } else {
        proc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    }
    return proc;
}

BOOST_PROCESS_V2_DECL
BOOL is_x86_process(HANDLE proc) {
    BOOL isWow = true;
    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        return isWow;
    if (IsWow64Process(proc, &isWow))
        return isWow;
    return isWow;
}

#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#define BOOST_PROCESS_V2_IMPL_DETAIL_EXE_IPP

