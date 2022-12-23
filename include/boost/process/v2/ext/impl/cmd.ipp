// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_CMD_IPP
#define BOOST_PROCESS_V2_IMPL_CMD_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/ext/detail/proc_info.hpp>
#include <boost/process/v2/ext/cmd.hpp>

#include <string>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#include <shellapi.h>
#else
#include <climits>
#endif

#if (defined(BOOST_PROCESS_V2_WINDOWS) || defined(__linux__) || defined(__ANDROID__)
#include <cstdlib>
#include <cstdio>
#endif

#if defined(__FreeBSD__)
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/user.h>
#include <libprocstat.h>
#endif

#if defined(__NetBSD__)
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#if defined(__OpenBSD__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#endif

#if defined(__DragonFly__)
#include <sys/types.h>
#include <kvm.h>
#endif

#if defined(__sun)
#include <kvm.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

#if defined(BOOST_PROCESS_V2_WINDOWS)

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    HANDLE proc = boost::process::v2::detail::ext::open_process_with_debug_privilege(pid, ec);
    if (proc == nullptr) {ec = detail::get_last_error(); return vec;}
    if (boost::process::v2::detail::ext::is_x86_process(GetCurrentProcess(), ec) != boost::process::v2::detail::ext::is_x86_process(proc, ec)) 
    {
        CloseHandle(proc); 
        return vec;
    } 
    else 
    {
        goto err;
    }
    int cmdsize = 0;
    std::vector<wchar_t> buffer = boost::process::v2::detail::ext::cwd_cmd_env_from_proc(proc, 0/*=MEMCMD*/, ec);
    if (!buffer.empty()) {
        wchar_t **cmd = CommandLineToArgvW(&buffer[0], &cmdsize);
        if (cmd) 
        {
            for (int i = 0; i < cmdsize; i++) 
            {
                vec.push_back(narrow(cmd[i]));
            }
            LocalFree(cmd);
        } else {
            goto err;
        }
    } else {
        goto err;
    }
    CloseHandle(proc);
    return vec;
err:
    CloseHandle(proc);
    ec = detail::get_last_error();  
    return vec;
}
    
#elif (defined(__APPLE__) && defined(__MACH__))

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    vec = boost::process::v2::detail::ext::cmd_env_from_proc_id(pid, 0/*=MEMCMD*/, ec);
    if (!vec.size()) {ec = detail::get_last_error();}
    return vec;
}
    
#elif (defined(__linux__) || defined(__ANDROID__))

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    FILE *file = fopen(("/proc/" + std::to_string(pid) + "/cmdline").c_str(), "rb");
    if (file) 
    {
        char *cmd = nullptr;
        std::size_t size = 0;
        while (getdelim(&cmd, &size, 0, file) != -1) 
        {
            vec.push_back(cmd);
        }
        while (!vec.empty() && vec.back().empty())
            vec.pop_back();
        if (cmd) free(cmd);
        fclose(file);
    } 
    else
        ec = detail::get_last_error();
    return vec;
}
    
#elif defined(__FreeBSD__)

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    unsigned cntp = 0;
    procstat *proc_stat = procstat_open_sysctl();
    if (proc_stat) 
    {
        kinfo_proc *proc_info = procstat_getprocs(proc_stat, KERN_PROC_PID, pid, &cntp);
        if (proc_info) 
        {
            char **cmd = procstat_getargv(proc_stat, proc_info, 0);
            if (cmd) 
            {
                for (int i = 0; cmd[i]; i++) 
                {
                    vec.push_back(cmd[i]);
                }
                procstat_freeargv(proc_stat);
            }
            else
			    ec = detail::get_last_error();
            procstat_freeprocs(proc_stat, proc_info);
        }
        else
            ec = detail::get_last_error();
        procstat_close(proc_stat);
    }
    else
        ec = detail::get_last_error();
    return vec;
}
    
#elif defined(__DragonFly__)

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    const char *nlistf, *memf;
    nlistf = memf = "/dev/null";
    boost::process::v2::detail::ext::kd = kvm_openfiles(nlistf, memf, nullptr, O_RDONLY, nullptr);
    if (!boost::process::v2::detail::ext::kd) {ec = detail::get_last_error(); return vec;}
    if ((proc_info = kvm_getprocs(boost::process::v2::detail::ext::kd, KERN_PROC_PID, pid, &cntp))) 
    {
        char **cmd = kvm_getargv(boost::process::v2::detail::ext::kd, proc_info, 0);
        if (cmd) 
        {
            for (int i = 0; cmd[i]; i++) 
            {
                vec.push_back(cmd[i]);
            }
        }
        else
            ec = detail::get_last_error();
    }
    else
        ec = detail::get_last_error();
    kvm_close(boost::process::v2::detail::ext::kd);
    return vec;
}
    
#elif defined(__NetBSD__)

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    int cntp = 0;
    kinfo_proc2 *proc_info = nullptr;
    boost::process::v2::detail::ext::kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!boost::process::v2::detail::ext::kd) {ec = detail::get_last_error(); return vec;}
    if ((proc_info = kvm_getproc2(boost::process::v2::detail::ext::kd, KERN_PROC_PID, pid, sizeof(struct kinfo_proc2), &cntp))) 
    {
        char **cmd = kvm_getargv2(boost::process::v2::detail::ext::kd, proc_info, 0);
        if (cmd) 
        {
            for (int i = 0; cmd[i]; i++) 
            {
                vec.push_back(cmd[i]);
            }
        }
        else
            ec = detail::get_last_error();
    }
    else
        ec = detail::get_last_error();
    kvm_close(boost::process::v2::detail::ext::kd);
    return vec;
}
    
#elif defined(__OpenBSD__)

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    boost::process::v2::detail::ext::kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!boost::process::v2::detail::ext::kd) {ec = detail::get_last_error(); return vec;}
    if ((proc_info = kvm_getprocs(boost::process::v2::detail::ext::kd, KERN_PROC_PID, pid, sizeof(struct kinfo_proc), &cntp))) 
    {
        char **cmd = kvm_getargv(boost::process::v2::detail::ext::kd, proc_info, 0);
        if (cmd) 
        {
            for (int i = 0; cmd[i]; i++) 
            {
                vec.push_back(cmd[i]);
            }
        }
        else
            ec = detail::get_last_error();
    }
    else
        ec = detail::get_last_error();
    kvm_close(boost::process::v2::detail::ext::kd);
    return vec;
}
    
#elif defined(__sun)

std::vector<std::string> commandline(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::vector<std::string> vec;
    char **cmd = nullptr;
    proc *proc_info = nullptr;
    user *proc_user = nullptr;
    boost::process::v2::detail::ext::kd = kvm_open(nullptr, nullptr, nullptr, O_RDONLY, nullptr);
    if (!boost::process::v2::detail::ext::kd) {ec = detail::get_last_error(); return vec;}
    if ((proc_info = kvm_getproc(boost::process::v2::detail::ext::kd, pid))) 
    {
        if ((proc_user = kvm_getu(boost::process::v2::detail::ext::kd, proc_info))) 
        {
            if (!kvm_getcmd(boost::process::v2::detail::ext::kd, proc_info, proc_user, &cmd, nullptr)) 
            {
                for (int i = 0; cmd[i]; i++) 
                {
                    vec.push_back(cmd[i]);
                }
                free(cmd);
            }
            else
                ec = detail::get_last_error();
        }
        else
            ec = detail::get_last_error();
    }
    else
        ec = detail::get_last_error();
    
    kvm_close(boost::process::v2::detail::ext::kd);
    return vec;
}

#else
#error "Platform not supported"
#endif

std::vector<std::string> commandline(boost::process::v2::pid_type pid)
{
        boost::system::error_code ec;
        auto res = commandline(pid, ec);
        if (ec)
                detail::throw_error(ec, "commandline");
        return res;
}

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_CMD_IPP

