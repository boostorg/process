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

#ifdef BOOST_PROCESS_USE_STD_FS
namespace filesystem = std::filesystem;
#else
namespace filesystem = boost::filesystem;
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace ext {

#if defined(BOOST_PROCESS_V2_WINDOWS)

filesystem::path executable(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    HANDLE proc = nullptr;
    if (pid == GetCurrentProcessId()) 
    {
        wchar_t buffer[MAX_PATH];
        if (GetModuleFileNameW(nullptr, buffer, sizeof(buffer))) 
        {
            wchar_t exe[MAX_PATH];
            if (_wfullpath(exe, buffer, MAX_PATH)) 
            {
                return exe;
            }
            else
                goto err;
        }
    } 
    else 
    {
        proc = detail::ext::open_process_with_debug_privilege(pid, ec);
        if (proc == nullptr) goto err;
        wchar_t buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (QueryFullProcessImageNameW(proc, 0, buffer, &size)) 
        {
            wchar_t exe[MAX_PATH];
            if (_wfullpath(exe, buffer, MAX_PATH)) 
            {
                CloseHandle(proc);
                return exe;
            }
            else
                goto err;
        }
    }
err:
    if (proc) 
        CloseHandle(proc);
    ec = detail::get_last_error();
    return "";
}

#elif (defined(__APPLE__) && defined(__MACH__))

filesystem::path executable(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    char exe[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, exe, sizeof(exe)) > 0) 
    {
        char buffer[PATH_MAX];
        if (realpath(exe, buffer)) 
        {
            return buffer;
        }
    }
    ec = detail::get_last_error();   
    return "";
}

#elif (defined(__linux__) || defined(__ANDROID__) || defined(__sun))

filesystem::path executable(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    std::string path;
    char exe[PATH_MAX];
#if (defined(__linux__) || defined(__ANDROID__))
    if (realpath(("/proc/" + std::to_string(pid) + "/exe").c_str(), exe))
#elif defined(__sun)
    if (realpath(("/proc/" + std::to_string(pid) + "/path/a.out").c_str(), exe))
#endif
        path = exe;
    else
        ec = detail::get_last_error();
    return path;
}

#elif (defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__))

filesystem::path executable(boost::process::v2::pid_type pid, boost::system::error_code & ec) 
{
    std::string path;
#if (defined(__FreeBSD__) || defined(__DragonFly__))
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, pid};
#elif defined(__NetBSD__)
    int mib[4] = {CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_PATHNAME};
#endif
    std::size_t len = 0;
    if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0) 
    {
        std::vector<char> strbuff;
        strbuff.resize(len, '\0');
        char *exe = strbuff.data();
        if (sysctl(mib, 4, exe, &len, nullptr, 0) == 0) 
        {
            char buffer[PATH_MAX];
            if (realpath(exe, buffer)) 
            {
                return buffer;
            }
            else
                goto err;
        }
        else
            goto err;
    }
err:
    ec = detail::get_last_error();
    return "";
}

#elif defined(__OpenBSD__)

filesystem::path executable(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    std::string path;
    auto is_executable = [](boost::process::v2::pid_type pid, std::string in, std::string *out, boost::system::error_code & ec) 
    {
        *out = "";
        bool success = false;
        struct stat st;
        char exe[PATH_MAX];
        if (!stat(in.c_str(), &st) && 
            (st.st_mode & S_IXUSR) && 
            (st.st_mode & S_IFREG) && 
            realpath(in.c_str(), exe)) 
        {
            int cntp = 0;
            kinfo_file *kif = nullptr;
            kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
            if (!kd)
                goto err;
            if ((kif = kvm_getfiles(kd, KERN_FILE_BYPID, pid, sizeof(struct kinfo_file), &cntp))) 
            {
                for (int i = 0; i < cntp; i++) 
                {
                    if (kif[i].fd_fd == KERN_FILE_TEXT && 
                        (st.st_dev == static_cast<dev_t>(kif[i].va_fsid) || 
                        st.st_ino == static_cast<ino_t>(kif[i].va_fileid))) 
                    {
                        *out = exe;
                        success = true;
                        break;
                    }
                }
            } 
            else
                goto err;
            kvm_close(kd);
        }
err:
        ec = detail::get_last_error();
        return success;
    };
    std::vector<std::string> buffer = cmdline_from_proc_id(pid, ec);
    if (!buffer.empty()) 
    {
        bool is_exe = false;
        std::string argv0;
        if (!buffer[0].empty()) 
        {
            if (buffer[0][0] == '/') 
            {
                argv0 = buffer[0];
                is_exe = is_executable(pid, argv0.c_str(), &path, ec);
            } 
            else if (buffer[0].find('/') == std::string::npos) 
            {
                std::string penv = envvar_value_from_proc_id(pid, "PATH");
                if (!penv.empty()) {
                    std::vector<std::string> env;
                    boost::split(env, penv, boost::is_any_of(":"));
                    for (std::size_t i = 0; i < env.size(); i++) 
                    {
                        argv0 = env[i] + "/" + buffer[0];
                        is_exe = is_executable(pid, argv0.c_str(), &path, ec);
                        if (is_exe) break;
                        if (buffer[0][0] == '-') 
                        {
                            argv0 = env[i] + "/" + buffer[0].substr(1);
                            is_exe = is_executable(pid, argv0.c_str(), &path, ec);
                            if (is_exe) break;
                        }
                    }
                }
            } else {
                std::string pwd = envvar_value_from_proc_id(pid, "PWD", ec);
                if (!pwd.empty()) 
                {
                    argv0 = pwd + "/" + buffer[0];
                    is_exe = is_executable(pid, argv0.c_str(), &path, ec);
                }
                if (pwd.empty() || !is_exe) 
                {
                    std::string cwd = current_path(pid, ec).string();
                    if (!cwd.empty()) 
                    {
                       argv0 = cwd + "/" + buffer[0];
                       is_exe = is_executable(pid, argv0.c_str(), &path, ec);
                    }
                }
            }
        }
    }
    if (path.empty())
        ec = detail::get_last_error();
    return path;
}

#else
#error "Platform not supported"
#endif

filesystem::path executable(boost::process::v2::pid_type pid)
{
    boost::system::error_code ec;
    auto res = executable(pid, ec);
    if (ec)
        detail::throw_error(ec, "executable");
    return res;
}

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_EXE_IPP

