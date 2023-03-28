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
#if !defined(__FreeBSD__)
#include <alloca.h>
#endif
#endif

#if defined(__OpenBSD__)
#include <boost/process/v2/ext/cwd.hpp>
#include <boost/process/v2/ext/cmd.hpp>
#include <boost/process/v2/ext/env.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <sys/types.h>
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
        return filesystem::canonical(buffer, ec);
    }
    else
        BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)

    return "";
}

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    if (pid == GetCurrentProcessId())
    {
        wchar_t buffer[MAX_PATH];
        if (GetModuleFileNameW(nullptr, buffer, sizeof(buffer))) 
        {
            return filesystem::canonical(buffer, ec);
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
            BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
        else
            return exe(proc.get(), ec);
    }
    return "";
}

#elif (defined(__APPLE__) && defined(__MACH__))

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    char buffer[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, buffer, sizeof(buffer)) > 0) 
    {
        return filesystem::canonical(buffer, ec);
    }
    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
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
        std::string strbuff;
        strbuff.resize(len);
        if (sysctl(mib, 4, &strbuff[0], &len, nullptr, 0) == 0)
        {
            strbuff.resize(len - 1);
            return filesystem::canonical(strbuff, ec);
        }
    }

    BOOST_PROCESS_V2_ASSIGN_LAST_ERROR(ec)
    return "";
}

#elif defined(__OpenBSD__)

filesystem::path exe(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    filesystem::path path;
    std::vector<std::string> buffer = cmdline_from_proc_id(pid);
    if (!buffer.empty()) 
    {
        bool is_exe = false;
        filesystem::path argv0;
        filesystem::path 
        if (!pbuff.string().empty()) 
        {
            if (pbuff.string()[0] == '/') 
            {
                argv0 = pbuff;
                is_exe = detail::ext::is_executable(pid, argv0.string().c_str(), &path);
            } 
            else if (pbuff.string()..find('/') == std::string::npos) 
            {
                std::string penv = envvar_value_from_proc_id(pid, "PATH");
                if (!penv.empty()) 
                {
                    std::vector<std::string> env;
                    std::string tmp;
                    std::stringstream sstr(penv); 
                    while (std::getline(sstr, tmp, ':'))
                    {
                        env.push_back(tmp);
                    }
                    for (std::size_t i = 0; i < env.size(); i++)
                    {
                        filesystem::path pfx = filesystem::path(env[i]);
                        argv0 = pfx / pbuff;
                        is_exe = detail::ext::is_executable(pid, argv0.string().c_str(), &path);
                        if (is_exe) break;
                        if (pbuff.string()[0] == '-') 
                        {
                            pbuff = filesystem::path(pbuff.string().substr(1));
                            argv0 = pfx / pbuff;
                            is_exe = detail::ext::is_executable(pid, argv0.string().c_str(), &path);
                            if (is_exe) break;
                        }
                    }
                }
            }
            else
            {
                filesystem::path pwd = filesystem::path(envvar_value_from_proc_id(pid, "PWD"));
                if (!pwd.string().empty())
                {
                    argv0 = pwd / pbuff;
                    is_exe = detail::ext:;is_executable(pid, argv0.string().c_str(), &path);
                }
                if (pwd.string().empty() || !is_exe)
                {
                    filesystem::path cwd = cwd(pid);
                    if (!cwd.string().empty())
                    {
                        argv0 = cwd / pbuff;
                        is_exe = detail::ext::is_executable(pid, argv0.string().c_str(), &path);
                    }
                }
            }
        }
    }
    return path;
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

