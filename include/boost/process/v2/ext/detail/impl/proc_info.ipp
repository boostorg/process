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
std::vector<wchar_t> cwd_cmd_env_from_proc(HANDLE proc, int type, boost::system::error_code & ec) {
    std::vector<wchar_t> buffer;
    PEB peb;
    SIZE_T nRead = 0; 
    ULONG len = 0;
    PROCESS_BASIC_INFORMATION pbi;
    RTL_USER_PROCESS_PARAMETERS upp;
    wchar_t *res = nullptr;
    FARPROC farProc = nullptr;
    NTSTATUS status = 0;
    PVOID buf = nullptr;
    typedef NTSTATUS (__stdcall *NTQIP)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
    NTQIP NtQueryInformationProcess;
    HMODULE hModule = GetModuleHandleW(L"ntdll.dll");
    if (!hModule) { goto err; }
    farProc = GetProcAddress(hModule, "NtQueryInformationProcess");
    if (!farProc) { goto err; }
    NtQueryInformationProcess = (NTQIP)farProc;
    status = NtQueryInformationProcess(proc, ProcessBasicInformation, &pbi, sizeof(pbi), &len);
    if (status) { goto err; }
    ReadProcessMemory(proc, pbi.PebBaseAddress, &peb, sizeof(peb), &nRead);
    if (!nRead) { goto err; }
    ReadProcessMemory(proc, peb.ProcessParameters, &upp, sizeof(upp), &nRead);
    if (!nRead) { goto err; }
    len = 0;
    if (type == MEMCWD) {
        buf = upp.CurrentDirectory.DosPath.Buffer;
        len = upp.CurrentDirectory.DosPath.Length;
    } else if (type == MEMENV) {
        buf = upp.Environment;
        len = (ULONG)upp.EnvironmentSize;
    } else if (type == MEMCMD) {
        buf = upp.CommandLine.Buffer;
        len = upp.CommandLine.Length;
    }
    res = new wchar_t[len / 2 + 1];
    ReadProcessMemory(proc, buf, res, len, &nRead);
    if (!nRead) { delete[] res; goto err; }
    res[len / 2] = L'\0';
    std::copy(res, res + (len / 2 + 1), std::back_inserter(buffer));
    delete[] res;
    return buffer;
err:
    ec = detail::get_last_error();
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

#elif (defined(__APPLE__) && (__MACH__))
// type of process memory to read?
enum MEMTYP {MEMCMD, MEMENV}; // FIXME: Add MEMEXE using <sys/sysctl.h> and not <libproc.h>
std::vector<std::string> cmd_env_from_proc_id(ngs::xproc::PROCID proc_id, int type, boost::system::error_code & ec)
{
    std::vector<std::string> vec;
    std::size_t s = 0;
    int argmax = 0, nargs = 0;
    char *procargs = nullptr, *sp = nullptr, *cp = nullptr; 
    int mib[3];
    mib[0] = CTL_KERN;
    mib[1] = KERN_ARGMAX;
    s = sizeof(argmax);
    if (sysctl(mib, 2, &argmax, &s, nullptr, 0) == -1)
    {
        ec = detail::get_last_error();
        return vec;
    }
    procargs = (char *)malloc(argmax);
    if (procargs == nullptr)
    {
        ec = detail::get_last_error();
        return vec;
    }
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROCARGS2;
    mib[2] = proc_id;
    s = argmax;
    if (sysctl(mib, 3, procargs, &s, nullptr, 0) == -1)
    {
        ec = detail::get_last_error();
        free(procargs);
        return vec;
    }
    memcpy(&nargs, procargs, sizeof(nargs));
    cp = procargs + sizeof(nargs);
    for (; cp < &procargs[s]; cp++)
    {
        if (*cp == '\0') break;
    }
    if (cp == &procargs[s])
    {
        ec = detail::get_last_error();
        free(procargs); 
        return vec;
    }
    for (; cp < &procargs[s]; cp++)
    {
        if (*cp != '\0') break;
    }
    if (cp == &procargs[s])
    {
        ec = detail::get_last_error();
        free(procargs);
        return vec;
    }
    sp = cp;
    int i = 0, j = 0;
    while (*sp != '\0' && sp < &procargs[s])
    {
        if (type && j >= nargs) 
        { 
            vec.push_back(sp); i++;
        } 
        else if (!type && j < nargs) 
        {
            vec.push_back(sp); i++;
        }
        sp += strlen(sp) + 1; j++;
    }
    free(procargs);
    return vec;
}
#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP

