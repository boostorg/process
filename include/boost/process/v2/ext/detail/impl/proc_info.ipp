// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP
#define BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/ext/detail/proc_info.hpp>
#include <string>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <iterator>
#include <algorithm>
#include <windows.h>
#elif (defined(__APPLE__) && defined(__MACH__))
#include <cstdlib>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/proc_info.h>
#include <libproc.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

namespace ext
{

#if defined(BOOST_PROCESS_V2_WINDOWS)
// type of process memory to read?
enum MEMTYP {MEMCMD, MEMENV, MEMCWD};
std::wstring cwd_cmd_env_from_proc(HANDLE proc, int type, boost::system::error_code & ec)
{
    std::wstring buffer;
    PEB peb;
    SIZE_T nRead = 0; 
    ULONG len = 0;
    PROCESS_BASIC_INFORMATION pbi;
    RTL_USER_PROCESS_PARAMETERS upp;

    wchar_t *res = nullptr;
    FARPROC farProc = nullptr;
    NTSTATUS status = 0;
    PVOID buf = nullptr;
    status = NtQueryInformationProcess(proc, ProcessBasicInformation, &pbi, sizeof(pbi), &len);
    ULONG error = RtlNtStatusToDosError(status);

    if (error)
    {
        ec.assign(error, boost::system::system_category());
        return {};
    }

    if (!ReadProcessMemory(proc, pbi.PebBaseAddress, &peb, sizeof(peb), &nRead))
    {
        ec = detail::get_last_error();
        return {};
    }

    if (!ReadProcessMemory(proc, peb.ProcessParameters, &upp, sizeof(upp), &nRead))
    {
        ec = detail::get_last_error();
        return {};
    }
    len = 0;
    if (type == MEMCWD)
    {
        buf = upp.CurrentDirectory.DosPath.Buffer;
        len = upp.CurrentDirectory.DosPath.Length;
    }
    else if (type == MEMENV)
    {
        buf = upp.Environment;
        len = (ULONG)upp.EnvironmentSize;
    }
    else if (type == MEMCMD)
    {
        buf = upp.CommandLine.Buffer;
        len = upp.CommandLine.Length;
    }

    buffer.resize(len / 2 + 1);
    if (!ReadProcessMemory(proc, buf, &buffer[0], len, &nRead))
    {
        ec = detail::get_last_error();
        return {};
    }
    buffer[len / 2] = L'\0';
    return buffer;
}

// with debug_privilege enabled allows reading info from more processes
// this includes stuff such as exe path, cwd path, cmdline, and environ
HANDLE open_process_with_debug_privilege(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    HANDLE proc = nullptr;
    HANDLE hToken = nullptr;
    LUID luid;
    TOKEN_PRIVILEGES tkp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        if (LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid))
        {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Luid = luid;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            if (AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), nullptr, nullptr))
            {
                proc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
            }
        }
        CloseHandle(hToken);
    }
    if (!proc)
        proc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!proc)
        ec = detail::get_last_error();
    return proc;
}

// NOTE: Windows-only limitation described right here
// cwd, cmdline, and environ can only be read from by
// the calling process when the platform architecture 
// with the target process matches both the processes
BOOL is_x86_process(HANDLE proc, boost::system::error_code & ec) {
    BOOL isWow = true;
    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        return isWow;
    if (IsWow64Process(proc, &isWow))
        return isWow;
    else
        ec = detail::get_last_error();
    return isWow;
}
#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP

