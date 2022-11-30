// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_EXE_IPP
#define BOOST_PROCESS_V2_IMPL_EXE_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/detail/exe.hpp>
#include <boost/process/v2/exe.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#else
#include <climits>

#if (defined(__APPLE__) && defined(__MACH__))
#include <libproc.h>
#endif

#if (defined(BOOST_PROCESS_V2_WINDOWS) || defined(__linux__) || defined(__ANDROID__) || defined(__sun))
#include <cstdlib>
#endif

#if (defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__))
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#if defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

#if defined(BOOST_PROCESS_V2_WINDOWS)

boost::filesystem::path exe_path(pid_type pid, error_code & ec)
{
    boost::filesystem::path path;
    if (pid == GetCurrentProcessId()) 
    {
        wchar_t buffer[MAX_PATH];
        if (GetModuleFileNameW(nullptr, buffer, sizeof(buffer)) != 0) 
        {
            wchar_t exe[MAX_PATH];
            if (_wfullpath(exe, buffer, MAX_PATH)) 
            {
                path = exe;
            }
        }
    } 
    else 
    {
        HANDLE proc = detail::ext::open_process_with_debug_privilege(pid);
        if (proc == nullptr) return path;
        wchar_t buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (QueryFullProcessImageNameW(proc, 0, buffer, &size) != 0) 
        {
            wchar_t exe[MAX_PATH];
            if (_wfullpath(exe, buffer, MAX_PATH)) 
            {
                path = exe;
            }
        }
        CloseHandle(proc);
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#elif (defined(__APPLE__) && defined(__MACH__))

boost::filesystem::path exe_path(pid_type pid, error_code & ec)
{
    boost::filesystem::path;
    char exe[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, exe, sizeof(exe)) > 0) 
    {
        char buffer[PATH_MAX];
        if (realpath(exe, buffer)) 
        {
            path = buffer;
        }
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#elif (defined(__linux__) || defined(__ANDROID__))

boost::filesystem::path exe_path(pid_type pid, error_code & ec)
{
    boost::filesystem::path;
    char exe[PATH_MAX];
    if (realpath(("/proc/" + std::to_string(pid) + "/exe").c_str(), exe)) 
    {
        path = exe;
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#elif defined(__FreeBSD__) || defined(__DragonFly__)

boost::filesystem::path exe_path(pid_type pid, error_code & ec) 
{
    boost::filesystem::path;
    int mib[4]; 
    std::size_t len;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = pid;
    if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0) 
    {
        std::string strbuff;
        strbuff.resize(len, '\0');
        char *exe = strbuff.data();
        if (sysctl(mib, 4, exe, &len, nullptr, 0) == 0) 
        {
            char buffer[PATH_MAX];
            if (realpath(exe, buffer)) 
            {
                path = buffer;
            }
        }
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#elif defined(__NetBSD__)

boost::filesystem::path exe_path(pid_type pid, error_code & ec)
{
    boost::filesystem::path;
    int mib[4]; 
    std::size_t len;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC_ARGS;
    mib[2] = pid;
    mib[3] = KERN_PROC_PATHNAME;
    if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0)
    {
        std::string strbuff;
        strbuff.resize(len, '\0');
        char *exe = strbuff.data();
        if (sysctl(mib, 4, exe, &len, nullptr, 0) == 0)
        {
            char buffer[PATH_MAX];
            if (realpath(exe, buffer))
            {
                path = buffer;
            }
        }
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#elif defined(__OpenBSD__)

boost::filesystem::path exe_path(pid_type pid, error_code & ec)
{
    boost::filesystem::path;
    auto is_executable = [](pid_type pid, std::string in, std::string *out) 
    {
        *out = "";
        bool success = false;
        struct stat st;
        char executable[PATH_MAX];
        if (!stat(in.c_str(), &st) && (st.st_mode & S_IXUSR) && (st.st_mode & S_IFREG) && realpath(in.c_str(), executable)) 
        {
            int cntp = 0;
            kinfo_file *kif = nullptr;
            kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
            if (!kd) return false;
            if ((kif = kvm_getfiles(kd, KERN_FILE_BYPID, pid, sizeof(struct kinfo_file), &cntp))) 
            {
                for (int i = 0; i < cntp; i++) 
                {
                    if (kif[i].fd_fd == KERN_FILE_TEXT && 
                        (st.st_dev == static_cast<dev_t>(kif[i].va_fsid) || st.st_ino == static_cast<ino_t>(kif[i].va_fileid))) 
                    {
                        *out = executable;
                        success = true;
                        break;
                    }
                }
            }
            kvm_close(kd);
        }
        return success;
    };
    std::vector<std::string> buffer = cmdline_from_proc_id(pid);
    if (!buffer.empty()) 
    {
        bool is_exe = false;
        std::string argv0;
        if (!buffer[0].empty()) 
        {
            if (buffer[0][0] == '/') 
            {
                argv0 = buffer[0];
                is_exe = is_executable(pid, argv0.c_str(), &path);
            } 
            else if (buffer[0].find('/') == std::string::npos) 
            {
                std::string penv = envvar_value_from_proc_id(pid, "PATH");
                if (!penv.empty()) {
                    std::vector<std::string> env;
                    std::string tmp;
                    std::stringstream sstr(penv); 
                    while (std::getline(sstr, tmp, ':')) 
                    {
                        env.push_back(tmp);
                    }
                    for (std::size_t i = 0; i < env.size(); i++) 
                    {
                        argv0 = env[i] + "/" + buffer[0];
                        is_exe = is_executable(pid, argv0.c_str(), &path);
                        if (is_exe) break;
                        if (buffer[0][0] == '-') 
                        {
                            argv0 = env[i] + "/" + buffer[0].substr(1);
                            is_exe = is_executable(pid, argv0.c_str(), &path);
                            if (is_exe) break;
                        }
                    }
                }
            } else {
                std::string pwd = envvar_value_from_proc_id(pid, "PWD");
                if (!pwd.empty()) 
                {
                    argv0 = pwd + "/" + buffer[0];
                    is_exe = is_executable(pid, argv0.c_str(), &path);
                }
                if (!is_exe) 
                {
                    std::string cwd = cwd_from_proc_id(pid);
                    if (!cwd.empty()) 
                    {
                       argv0 = cwd + "/" + buffer[0];
                       is_exe = is_executable(pid, argv0.c_str(), &path);
                    }
                }
            }
        }
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#elif defined(__sun)

boost::filesystem::path exe_path(pid_type pid, error_code & ec)
{
    boost::filesystem::path;
    char exe[PATH_MAX];
    if (realpath(("/proc/" + std::to_string(pid) + "/path/a.out").c_str(), exe)) 
    {
        path = exe;
    }
    if (path.string().empty())
    {
        ec = detail::get_last_error();
    }
    return path;
}

#else
#error "Platform not supported"
#endif

boost::filesystem::path exe_path(pid_type pid)
{
    error_code ec;
    auto res = exe_path(pid, ec);
    if (ec)
        detail::throw_error(ec, "all_pids");
    return res;
}

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_IMPL_EXE_IPP

