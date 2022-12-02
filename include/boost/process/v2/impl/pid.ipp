// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_PID_IPP
#define BOOST_PROCESS_V2_IMPL_PID_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/pid.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#endif

#if (defined(__APPLE__) && defined(__MACH__))
#include <sys/proc_info.h>
#include <libproc.h>
#endif

#if (defined(__linux__) || defined(__ANDROID__))
#include <dirent.h>
#endif

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/user.h>
#include <libutil.h>
#include <cstdlib>
#endif

#if defined(__DragonFly__)
#include <sys/types.h>
#include <kvm.h>
#endif

#if defined(__OpenBSD__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <kvm.h>
#endif

#if defined(__NetBSD__)
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#if defined(__sun)
#include <kvm.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/proc.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_V2_WINDOWS)
pid_type current_pid() {return ::GetCurrentProcessId();}
#else
pid_type current_pid() {return ::getpid();}
#endif

#if defined(BOOST_PROCESS_V2_WINDOWS)

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hp)
    {
        ec = detail::get_last_error();
        return vec;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hp, &pe)) 
    {
        do 
        {
            vec.push_back(pe.th32ProcessID);
        } while (Process32Next(hp, &pe));
    }
    CloseHandle(hp);
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec)
{
    std::vector<pid_type> vec;
    HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hp)
    {
        ec = detail::get_last_error();
        return vec;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hp, &pe))
    {
        do
        {
            if (pe.th32ProcessID == pid)
            {
                vec.push_back(pe.th32ParentProcessID);
                break;
            }
        }
        while (Process32Next(hp, &pe));
    }
    CloseHandle(hp);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec)
{
    std::vector<pid_type> vec;
    HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hp)
    {
        ec = detail::get_last_error();
        return vec;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hp, &pe))
    {
        do
        {
            if (pe.th32ParentProcessID == pid)
            {
                vec.push_back(pe.th32ProcessID);
            }
        } 
        while (Process32Next(hp, &pe));
    }
    CloseHandle(hp);
    return vec;
}

#elif (defined(__APPLE__) && defined(__MACH__))

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    vec.resize(proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0));
    if (proc_listpids(PROC_ALL_PIDS, 0, &vec[0], sizeof(pid_type) * vec.size()))
    {
        ec = detail::get_last_error();
        return {};
    }
    auto itr = std::partition(vec.begin(), vec.end(), [](pid_type pt) {return pt != 0;});
    vec.erase(itr, vec.end());
    vec.push_back(0);
    std::reverse(vec.begin(), vec.end());
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    proc_bsdinfo proc_info;
    if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &proc_info, sizeof(proc_info)) <= 0)
    {
        ec = detail::get_last_error();
        return {};
    }
    else
    {
        vec.push_back(proc_info.pbi_ppid);
    }
    if (vec.empty() && (pid == 0 || pid == 1))
        vec.push_back(0);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    vec.resize(proc_listpids(PROC_PPID_ONLY, (uint32_t)pid, nullptr, 0));
    if (proc_listpids(PROC_PPID_ONLY, (uint32_t)pid, &proc_info[0], sizeof(pid_type) * cntp))
    {
        ec = detail::get_last_error();
        return {};
    }
    auto itr = std::partition(vec.begin(), vec.end(), [](pid_type pt) {return pt != 0;});
    vec.erase(itr, vec.end());
    std::reverse(vec.begin(), vec.end());
    for (unsigned i = 0; i < vec.size(); i++)
    {
        if (proc_info[i] == 1 && pid == 0)
        {
            vec.insert(vec.begin(), 0);
            break;
        }
    }
    return vec;
}

#elif (defined(__linux__) || defined(__ANDROID__))

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    vec.push_back(0);
    DIR *proc = opendir("/proc");
    if (!proc)
    {
        ec = detail::get_last_error();
        return vec;
    } 
    struct dirent *ent = nullptr;
    while ((ent = readdir(proc))) 
    {
        if (!isdigit(*ent->d_name))
            continue;
        vec.push_back(atoi(ent->d_name));
    }
    closedir(proc);
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    char buffer[BUFSIZ];
    sprintf(buffer, "/proc/%d/stat", pid);
    FILE *stat = fopen(buffer, "r");
    if (!stat)
    {
        ec = detail::get_last_error();
        return vec;
    } 
    else
    {
        std::size_t size = fread(buffer, sizeof(char), sizeof(buffer), stat);
        if (size > 0) 
        {
            char *token = nullptr;
            if ((token = strtok(buffer, " "))) {
                if ((token = strtok(nullptr, " "))) {
                    if ((token = strtok(nullptr, " "))) {
                        if ((token = strtok(nullptr, " "))) {
                            pid_type ppid = (pid_type)strtoul(token, nullptr, 10);
                            vec.push_back(ppid);
                        }
                    }
                }
            }
            if (!token)
            {
                ec = detail::get_last_error();
                return vec;
            }
        }
        fclose(stat);
    }
    if (vec.empty() && pid == 0) 
        vec.push_back(0);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    std::vector<pid_type> pids = all_pids(ec);
    for (std::size_t i = 0; i < pids.size(); i++)
    {
        std::vector<pid_type> ppid = parent_pid(pids[i], ec);
        if (!ppid.empty() && ppid[0] == pid)
        {
            vec.push_back(pids[i]);
        }
    }
    return vec;
}

#elif defined(__FreeBSD__)

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc *proc_info = kinfo_getallproc(&cntp);
    if (proc_info) 
    {
        for (int i = 0; i < cntp; i++)
            vec.push_back(proc_info[i].ki_pid);
        free(proc_info);
    }
    else
        ec = detail::get_last_error();
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec)
{
    std::vector<pid_type> vec;
    kinfo_proc *proc_info = kinfo_getproc(pid);
    if (proc_info)
    {
        vec.push_back(proc_info->ki_ppid);
        free(proc_info);
    }
    else
        ec = detail::get_last_error();
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    int cntp = 0; 
    kinfo_proc *proc_info = kinfo_getallproc(&cntp);
    if (proc_info)
    {
        for (int i = 0; i < cntp; i++)
        {
            if (proc_info[i].ki_ppid == pid)
            {
                vec.push_back(proc_info[i].ki_pid);
            }
        }
        free(proc_info);
    }
    else
        ec = detail::get_last_error();
    return vec;
}

#elif defined(__DragonFly__)

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    const char *nlistf, *memf;
    nlistf = memf = "/dev/null";
    kd = kvm_openfiles(nlistf, memf, nullptr, O_RDONLY, nullptr);
    if (!kd) 
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getprocs(kd, KERN_PROC_ALL, 0, &cntp))) 
    {
        vec.reserve(cntp);
        for (int i = 0; i < cntp; i++) 
            if (proc_info[i].kp_pid >= 0) 
                vec.push_back(proc_info[i].kp_pid);
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec)
{
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    const char *nlistf, *memf;
    nlistf = memf = "/dev/null";
    kd = kvm_openfiles(nlistf, memf, nullptr, O_RDONLY, nullptr); 
    if (!kd) 
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getprocs(kd, KERN_PROC_PID, pid, &cntp)))
    {
        if (proc_info->kp_ppid >= 0)
        {
            vec.push_back(proc_info->kp_ppid);
        }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    if (vec.empty() && pid == 0)
        vec.push_back(0);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    const char *nlistf, *memf;
    nlistf = memf = "/dev/null";
    kd = kvm_openfiles(nlistf, memf, nullptr, O_RDONLY, nullptr);
    if (!kd) 
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getprocs(kd, KERN_PROC_ALL, 0, &cntp)))
    {
        for (int i = 0; i < cntp; i++)
        {
            if (proc_info[i].kp_pid == 1 && proc_info[i].kp_ppid == 0 && pid == 0)
            {
                vec.push_back(0);
            }
            if (proc_info[i].kp_pid >= 0 && proc_info[i].kp_ppid >= 0 && proc_info[i].kp_ppid == pid)
            {
                vec.push_back(proc_info[i].kp_pid);
            }
        }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

#elif defined(__NetBSD__)

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc2  *proc_info = nullptr;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!kd)
    {
        ec = detail::get_last_error();
        return vec;
    } 
    if ((proc_info = kvm_getproc2(kd, KERN_PROC_ALL, 0, sizeof(struct kinfo_proc2), &cntp)))
    {
        vec.reserve(cntp);
        for (int i = cntp - 1; i >= 0; i--) 
        {
            vec.push_back(proc_info[i].p_pid);
        }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc2 *proc_info = nullptr;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!kd) 
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getproc2(kd, KERN_PROC_PID, pid, sizeof(struct kinfo_proc2), &cntp)))
    {
        vec.push_back(proc_info->p_ppid);
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc2 *proc_info = nullptr;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!kd) 
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getproc2(kd, KERN_PROC_ALL, 0, sizeof(struct kinfo_proc2), &cntp)))
    {
        for (int i = cntp - 1; i >= 0; i--)
        {
            if (proc_info[i].p_ppid == pid)
            {
                vec.push_back(proc_info[i].p_pid);
            }
        }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

#elif defined(__OpenBSD__)

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    vec.push_back(0);
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!kd)
    {
        ec = detail::get_last_error();
        return vec;
    } 
    if ((proc_info = kvm_getprocs(kd, KERN_PROC_ALL, 0, sizeof(struct kinfo_proc), &cntp)))
    {
        vec.reserve(vec.size() + cntp);
        for (int i = cntp - 1; i >= 0; i--) {
        {
            if (proc_info[i].kp_pid >= 0) 
            {
                vec.push_back(proc_info[i].kp_pid);
            }
        }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!kd)
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getprocs(kd, KERN_PROC_PID, pid, sizeof(struct kinfo_proc), &cntp)))
    {
        vec.push_back(proc_info->p_ppid);
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    if (vec.empty() && pid == 0)
        vec.push_back(0);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    int cntp = 0;
    kinfo_proc *proc_info = nullptr;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr); 
    if (!kd) 
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getprocs(kd, KERN_PROC_ALL, 0, sizeof(struct kinfo_proc), &cntp)))
    {
        for (int i = cntp - 1; i >= 0; i--)
        {
            if (proc_info[i].p_pid == 1 && proc_info[i].p_ppid == 0 && pid == 0)
            {
                vec.push_back(0);
            }
            if (proc_info[i].p_ppid == pid)
            {
                vec.push_back(proc_info[i].p_pid);
            }
        }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}


#elif defined(__sun)

std::vector<pid_type> all_pids(std::error_code & ec)
{
    std::vector<pid_type> vec;
    struct pid cur_pid;
    proc *proc_info = nullptr;
    kd = kvm_open(nullptr, nullptr, nullptr, O_RDONLY, nullptr);
    if (!kd)
    {
        ec = detail::get_last_error();
        return vec;
    } 
    while ((proc_info = kvm_nextproc(kd)))
    {
        if (kvm_kread(kd, (std::uintptr_t)proc_info->p_pidp, &cur_pid, sizeof(cur_pid)) != -1)
        {
            vec.insert(vec.begin(), cur_pid.pid_id);
        }
        else
        {
            ec = detail::get_last_error();
            break;
        }
    }
    kvm_close(kd);
    return vec;
}

std::vector<pid_type> parent_pid(pid_type pid, std::error_code & ec) {
    std::vector<pid_type> vec;
    proc *proc_info = nullptr;
    kd = kvm_open(nullptr, nullptr, nullptr, O_RDONLY, nullptr);
    if (!kd)
    {
        ec = detail::get_last_error();
        return vec;
    }
    if ((proc_info = kvm_getproc(kd, pid)))
    {
        vec.push_back(proc_info->p_ppid);
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

std::vector<pid_type> child_pids(pid_type pid, std::error_code & ec)
{
    std::vector<pid_type> vec;
    struct pid cur_pid;
    proc *proc_info = nullptr;
    kd = kvm_open(nullptr, nullptr, nullptr, O_RDONLY, nullptr);
    if (!kd)
    {
        ec = detail::get_last_error();
        return vec;
    }
    while ((proc_info = kvm_nextproc(kd)))
    {
        if (proc_info->p_ppid == pid)
        {
            if (kvm_kread(kd, (std::uintptr_t)proc_info->p_pidp, &cur_pid, sizeof(cur_pid)) != -1)
            {
                vec.insert(vec.begin(), cur_pid.pid_id);
            }
            else
            {
                ec = detail::get_last_error();
                break;
            }
        }
    }
    kvm_close(kd);
    return vec;
}

#else
#error "Platform not supported"
#endif

std::vector<pid_type> all_pids()
{
    std::error_code ec;
    auto res = all_pids(ec);
    if (ec)
        detail::throw_error(ec, "all_pids");
    return res;
}

std::vector<pid_type> parent_pid(pid_type pid)
{
    std::error_code ec;
    auto res = parent_pid(pid, ec);
    if (ec)
        detail::throw_error(ec, "parent_pid");
    return res;
}

std::vector<pid_type> child_pids(pid_type pid)
{
    std::error_code ec;
    auto res = child_pids(pid, ec);
    if (ec)
        detail::throw_error(ec, "child_pids");
    return res;
}

BOOST_PROCESS_V2_END_NAMESPACE

#endif // BOOST_PROCESS_V2_IMPL_PID_IPP

