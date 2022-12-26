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

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#include <shellapi.h>
#else
#include <cstdlib>
#endif

#if (defined(__linux__) || defined(__ANDROID__))
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

struct make_cmd_shell_
{
#if defined(BOOST_PROCESS_V2_WINDOWS)
    static shell make(std::wstring data)
    {
        shell res;
        res.input_  = res.buffer_ = std::move(data);
        res.parse_();
        return res;
    }
#else
    static shell make(std::string data,
                      int argc, char ** argv,
                      void(*free_func)(int, char**))
    {
        shell res;
        res.argc_  = argc;
        res.input_ = res.buffer_ = std::move(data);
        res.argv_  = argv;
        res.free_argv_ = free_func;

        return res;
    }
#endif
};

namespace ext {

#if defined(BOOST_PROCESS_V2_WINDOWS)

shell cmd(HANDLE proc, error_code & ec)
{
    std::wstring buffer = boost::process::v2::detail::ext::cwd_cmd_env_from_proc(proc, 0/*=MEMCMD*/, ec);

    if (!ec)
        return make_cmd_shell_::make(std::move(buffer));
    else
        return {};
}

session cmd(boost::process::v2::pid_type pid, boost::system::error_code & ec)
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
        return cmd(proc.get(), ec);

}
    
#elif (defined(__APPLE__) && defined(__MACH__))

shell cmd(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    // attempt to do this here - @time-killer-games feel free to adjust, this wasn't compiled
    // we use two ptrs
    int mib[3] = {CTL_KERN, KERN_ARGMAG, 0};
    int argmax = 0;
    auto size = sizeof(argmax);
    if (sysctl(mib, 2, &argmax, &size, nullptr, 0) == -1)
    {
        ec = detail::get_last_error();
        return {};
    }

    std::string procargs;
    procargs.resize(argmax - 1);
    mib[1] = KERN_PROCARGS2;
    mib[2] = pid;

    size = argmax;

    if (sysctl(mib, 3, &*procargs.begin(), &size, nullptr, 0) != 0)
    {
        ec = detail::get_last_error();
        return {};
    }

    int argc = *reinterpret_cast<int*>(procargs.data());
    auto itr = procargs.begin() + sizeof(argc);

    std::unique_ptr<char*[]> argv{new char*[argc + 1]};
    const auto end = procargs.end();

    argv[argc] = nullptr; //args is a null-terminated list

    for (auto n = 0u; n <= argc; n++)
    {
        auto e = std::find(itr, end, '\0');
        if (e == end && n < argc) // something off
        {
            ec.assign(EINVAL, system_category());
            return {};
        }
        argv[n] = &*itr;
        itr = e + 1; // start searching start
    }

    auto fr_func = +[](int argc, char ** argv) {delete [] argv;};

    return make_cmd_shell_::make(std::move(procargs), argc, argv.release(), fr_func);
}
    
#elif (defined(__linux__) || defined(__ANDROID__))

shell cmd(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    std::string procargs;
    procargs.resize(4096);
    int f = ::open(("/proc/" + std::to_string(pid) + "/cmdline").c_str(), O_RDONLY);

    while (procargs.back() != EOF)
    {
        auto r = ::read(f, &*(procargs.end() - 4096), 4096);
        if (r < 0)
        {
            ec = detail::get_last_error();
            ::close(f);
            return {};
        }
        if (r < 4096) // done!
        {
            procargs.resize(procargs.size() - 4096 + r);
            break;
        }
        procargs.resize(procargs.size() + 4096);
    }
    ::close(f);

    if (procargs.back() == EOF)
        procargs.pop_back();

    auto argc = std::count(procargs.begin(), procargs.end(), '\0');

    auto itr = procargs.begin();

    std::unique_ptr<char*[]> argv{new char*[argc + 1]};
    const auto end = procargs.end();

    argv[argc] = nullptr; //args is a null-terminated list


    for (auto n = 0u; n <= argc; n++)
    {
        auto e = std::find(itr, end, '\0');
        if (e == end && n < argc) // something off
        {
            ec.assign(EINVAL, system_category());
            return {};
        }
        argv[n] = &*itr;
        itr = e + 1; // start searching start
    }

    auto fr_func = +[](int argc, char ** argv) {delete [] argv;};

    return make_cmd_shell_::make(std::move(procargs), argc, argv.release(), fr_func);
}
    
#elif defined(__FreeBSD__)

shell cmd(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    struct cl_proc_state
    {
        void operator()(procstat *proc_stat)
        {
            procstat_close(proc_stat);
        }
    };
    std::unique_ptr<procstat, cl_proc_stat> proc_stat{procstat_open_sysctl()};
    if (!proc_stat)
    {
        ec = detail::get_last_error();
        return {};
    }

    struct proc_info_close
    {
        procstat * proc_stat;

        void operator()(kinfo_proc & proc_info)
        {
            procstat_freeprocs(proc_stat, proc_info);
        }
    };

    int cntp;
    std::unique_ptr<kinfo_proc, proc_info_close> proc_info{
            procstat_getprocs(proc_stat, KERN_PROC_PID, pid, &cntp),
            proc_info_close{proc_stat.get()}};

    if (!proc_info)
    {
        ec = detail::get_last_error();
        return {};
    }

    char **cmd = procstat_getargv(proc_stat, proc_info, 0);
    if (!cmd)
    {
        ec = detail::get_last_error();
        return {};
    }
    struct free_argv
    {
        procstat * proc_stat;
        ~free_argv()
        {
            procstat_freeargv(proc_stat);
        }
    };


    proc_info.get_deleter().has_cmd = true;

    int argc = 0;
    std::size_t str_lengths = 0;
    for (auto c = cmd; *c != nullptr; c++)
    {
        argc++;
        str_lengths += (std::strlen(*c) + 1);
    }
    // yes, not the greatest solution.
    std::string buffer;
    buffer.resize(
            sizeof(int) * (argc + 1)
            + str_lengths);

    char ** argv = reinterpret_cast<char**>(&*buffer.begin());
    argv[argc] = nullptr;
    auto p = &buffer[sizeof(int) * (argc) + 1];

    for (int i = 0; i < argc; i++)
    {
        const auto ln = std::strlen(cmd[i]);
        argv[i] = std::strcpy(p, cmd[i]);
        p += (ln + 1);
    }

    return make_cmd_shell_::make(std::move(buffer),
                                 argc, argv, nullptr);
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

shell cmd(boost::process::v2::pid_type pid)
{
        boost::system::error_code ec;
        auto res = cmd(pid, ec);
        if (ec)
                detail::throw_error(ec, "cmd");
        return res;
}

} // namespace ext

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_CMD_IPP

