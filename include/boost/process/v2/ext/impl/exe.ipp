// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_EXE_IPP
#define BOOST_PROCESS_V2_IMPL_EXE_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/ext/detail/proc_info.hpp>
#include <boost/process/v2/ext/exe.hpp>

#include <string>
#include <vector>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#else
#include <climits>
#endif

#if (defined(__APPLE__) && defined(__MACH__))
#include <sys/proc_info.h>
#include <libproc.h>
#endif

#if (defined(BOOST_PROCESS_V2_WINDOWS) || defined(__linux__) || defined(__ANDROID__) || defined(__sun))
#include <cstdlib>
#endif

#if (defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__))
#include <sys/types.h>
#include <sys/sysctl.h>
#include <alloca.h>
#endif

#if defined(__OpenBSD__)
#include <boost/process/v2/ext/cwd.hpp>
#include <boost/process/v2/ext/cmd.hpp>
#include <boost/process/v2/ext/env.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

#if defined(BOOST_PROCESS_V2_WINDOWS)

filesystem::path exe(HANDLE process_handle)
{
    boost::system::error_code ec;
    auto res = exe(process_handle, ec);
    if (ec)
        detail::throw_error(ec, "exe");
    return res;
}


filesystem::path exe(HANDLE proc, boost::system::error_code & ec)
{
    wchar_t buffer[MAX_PATH];
    // On input, specifies the size of the lpExeName buffer, in characters.
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(proc, 0, buffer, &size))
    {
        wchar_t exe[MAX_PATH];
        if (_wfullpath(exe, buffer, MAX_PATH))
            return exe;
        else
            ec = detail::get_last_error();
    }
    else
        ec = detail::get_last_error();

    return "";
}

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    if (pid == GetCurrentProcessId())
    {
        wchar_t buffer[MAX_PATH];
        if (GetModuleFileNameW(nullptr, buffer, sizeof(buffer))) 
        {
            wchar_t exe[MAX_PATH];
            if (_wfullpath(exe, buffer, MAX_PATH)) 
                return exe;
            else
                ec = detail::get_last_error();
        }
    } 
    else 
    {
        struct del
        {
            void operator()(HANDLE h)
            {
                ::CloseHandle(h);
            };
        };
        std::unique_ptr<void, del> proc{detail::ext::open_process_with_debug_privilege(pid, ec)};
        if (proc == nullptr)
            ec = detail::get_last_error();
        else
            return exe(proc.get(), ec);
    }
    return "";
}

#elif (defined(__APPLE__) && defined(__MACH__))

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    char exe[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, exe, sizeof(exe)) > 0) 
    {
        char buffer[PATH_MAX];
        if (realpath(exe, buffer)) 
            return buffer;

    }
    ec = detail::get_last_error();   
    return "";
}

#elif (defined(__linux__) || defined(__ANDROID__) || defined(__sun))

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
#if (defined(__linux__) || defined(__ANDROID__))
    return filesystem::canonical(
            filesystem::path("/proc") / std::to_string(pid) / "exe", ec
            );
#elif defined(__sun)
    return fileystem::canonical(
            filesystem::path("/proc") / std::to_string(pid) / "path/a.out"
            );
#endif
}

#elif (defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__))

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
#if (defined(__FreeBSD__) || defined(__DragonFly__))
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, pid};
#elif defined(__NetBSD__)
    int mib[4] = {CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_PATHNAME};
#endif
    std::size_t len = 0;
    if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0) 
    {
        std::vector<char> strbuff;
        strbuff.resize(len);
        if (sysctl(mib, 4, strbuff.data(), &len, nullptr, 0) == 0)
        {
            char buffer[PATH_MAX];
            if (realpath(strbuff.data(), buffer))
                return buffer;
        }
    }

    ec = detail::get_last_error();
    return "";
}

#elif defined(__OpenBSD__)

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    ec.assign(ENOTSUP, boost::system::system_category());
    return "";
}

#else
#error "Platform not supported"
#endif

filesystem::path exe(boost::process::v2::pid_type pid)
{
    boost::system::error_code ec;
    auto res = exe(pid, ec);
    if (ec)
        detail::throw_error(ec, "exe");
    return res;
}


} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_EXE_IPP

