// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_PROC_INFO_HPP
#define BOOST_PROCESS_V2_DETAIL_PROC_INFO_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>

#include <boost/process/v2/pid.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#include <winternl.h>

#if !defined(_MSC_VER)
#pragma pack(push, 8)
#else
#include <pshpack8.h>
#endif

/* 
** CURDIR struct from:
** https://github.com/processhacker/phnt/
** CC BY 4.0 licence 
*/

#define CURDIR struct {\
    UNICODE_STRING DosPath;\
    HANDLE Handle;\
}

/* 
** RTL_DRIVE_LETTER_CURDIR struct from:
** https://github.com/processhacker/phnt/
** CC BY 4.0 licence 
*/

#define RTL_DRIVE_LETTER_CURDIR struct {\
    USHORT Flags;\
    USHORT Length;\
    ULONG TimeStamp;\
    STRING DosPath;\
}

/* 
** RTL_USER_PROCESS_PARAMETERS struct from:
** https://github.com/processhacker/phnt/
** CC BY 4.0 licence 
*/

#define RTL_USER_PROCESS_PARAMETERS struct {\
    ULONG MaximumLength;\
    ULONG Length;\
    ULONG Flags;\
    ULONG DebugFlags;\
    HANDLE ConsoleHandle;\
    ULONG ConsoleFlags;\
    HANDLE StandardInput;\
    HANDLE StandardOutput;\
    HANDLE StandardError;\
    CURDIR CurrentDirectory;\
    UNICODE_STRING DllPath;\
    UNICODE_STRING ImagePathName;\
    UNICODE_STRING CommandLine;\
    PVOID Environment;\
    ULONG StartingX;\
    ULONG StartingY;\
    ULONG CountX;\
    ULONG CountY;\
    ULONG CountCharsX;\
    ULONG CountCharsY;\
    ULONG FillAttribute;\
    ULONG WindowFlags;\
    ULONG ShowWindowFlags;\
    UNICODE_STRING WindowTitle;\
    UNICODE_STRING DesktopInfo;\
    UNICODE_STRING ShellInfo;\
    UNICODE_STRING RuntimeData;\
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[32];\
    ULONG_PTR EnvironmentSize;\
    ULONG_PTR EnvironmentVersion;\
    PVOID PackageDependencyData;\
    ULONG ProcessGroupId;\
    ULONG LoaderThreads;\
    UNICODE_STRING RedirectionDllName;\
    UNICODE_STRING HeapPartitionName;\
    ULONG_PTR DefaultThreadpoolCpuSetMasks;\
    ULONG DefaultThreadpoolCpuSetMaskCount;\
}

#if !defined(_MSC_VER)
#pragma pack(pop)
#else
#include <poppack.h>
#endif

#elif (defined(__APPLE__) && defined(__MACH__))
#include <string>
#include <vector>
#elif defined(__DragonFly__)
#include <sys/types.h>
#include <kvm.h>
#elif defined(__NetBSD__)
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(__OpenBSD__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#elif defined(__sun)
#include <kvm.h>
#include <fcntl.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

namespace ext 
{

#if defined(BOOST_PROCESS_V2_WINDOWS)
BOOST_PROCESS_V2_DECL std::vector<wchar_t> cwd_cmd_env_from_proc(HANDLE proc, int type, boost::system::error_code & ec);
BOOST_PROCESS_V2_DECL HANDLE open_process_with_debug_privilege(boost::process::v2::pid_type pid, boost::system::error_code & ec);
BOOST_PROCESS_V2_DECL BOOL is_x86_process(HANDLE proc, boost::system::error_code & ec);
#elif (defined(__APPLE__) && defined(__MACH__))
BOOST_PROCESS_V2_DECL std::vector<std::string> cmd_env_from_proc_id(boost::process::v2::pid_type pid, int type, boost::system::error_code & ec);
#elif defined(__DragonFly__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__sun)
kvm_t *kd = nullptr;
#endif

} // namespace ext

} // namespace detail

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_DETAIL_PROC_INFO_HPP

