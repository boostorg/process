// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP
#define BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/ext/detail/proc_info.hpp>

#include <string>

#if (defined(__APPLE__) && defined(__MACH__))
#include <cstdlib>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/proc_info.h>
#include <libproc.h>
#endif

#if defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

namespace ext
{

#if defined(BOOST_PROCESS_V2_WINDOWS)
// type of process memory to read?
enum MEMTYP {MEMCMD, MEMCWD};
std::wstring cwd_cmd_from_proc(HANDLE proc, int type, boost::system::error_code & ec)
{
    std::wstring buffer;
    PEB peb;
    SIZE_T nRead = 0; 
    ULONG len = 0;
    PROCESS_BASIC_INFORMATION pbi;
    RTL_USER_PROCESS_PARAMETERS_EXTENDED upp;

    NTSTATUS status = 0;
    PVOID buf = nullptr;
    status = NtQueryInformationProcess(proc, ProcessBasicInformation, &pbi, sizeof(pbi), &len);
    ULONG error = RtlNtStatusToDosError(status);

    if (error)
    {
        BOOST_PROCESS_V2_ASSIGN_EC(ec, error, boost::system::system_category())
        return {};
    }

    if (!ReadProcessMemory(proc, pbi.PebBaseAddress, &peb, sizeof(peb), &nRead))
    {
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
        return {};
    }

    if (!ReadProcessMemory(proc, peb.ProcessParameters, &upp, sizeof(upp), &nRead))
    {
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
        return {};
    }

    if (type == MEMCWD)
    {
        buf = upp.CurrentDirectory.DosPath.Buffer;
        len = upp.CurrentDirectory.DosPath.Length;
    }
    else if (type == MEMCMD)
    {
        buf = upp.CommandLine.Buffer;
        len = upp.CommandLine.Length;
    }

    buffer.resize(len / 2 + 1);

    if (!ReadProcessMemory(proc, buf, &buffer[0], len, &nRead))
    {
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
        return {};
    }

    buffer.pop_back();
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
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
    return proc;
}
#endif

#if defined(__OpenBSD__)
bool is_executable(boost::process::v2::pid_type pid, std::string in, filesystem::path *out, boost::system::error_code & ec)
{
    bool success = false;
    struct stat st;
    if (!stat(in.c_str(), &st) && (st.st_mode & S_IXUSR) && (st.st_mode & S_IFREG))
    {
        char executable[PATH_MAX];
        if (realpath(in.c_str(), executable))
        {
            int cntp = 0;
            kinfo_file *kif = nullptr;
            kvm_t *kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
            if (!kd) 
            {
                BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
                return false;
            }
            if ((kif = kvm_getfiles(kd, KERN_FILE_BYPID, pid, sizeof(struct kinfo_file), &cntp)))
            {
                for (int i = 0; i < cntp; i++)
                {
                    if (kif[i].fd_fd == KERN_FILE_TEXT)
                    {
                        if (st.st_nlink == 1)
                        {
                            if (st.st_dev == static_cast<dev_t>(kif[i].va_fsid) && 
                                st.st_ino == static_cast<ino_t>(kif[i].va_fileid))
                            {
                                *out = filesystem::path(executable);
                                success = true;
                                break;
                            }
                        }
                    }
                }
            } else {
                BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
            }
            kvm_close(kd);
        }
    }
    return success;
}
#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_DETAIL_PROC_INFO_IPP

