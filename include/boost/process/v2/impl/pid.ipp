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

#if (defined(__APPLE__) && defined(__MACH__))
#include <sys/proc_info.h>
#include <libproc.h>
#endif

#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_V2_WINDOWS)
pid_type current_pid() {return ::GetCurrentProcessId();}
#else
pid_type current_pid() {return ::getpid();}
#endif

#if defined(BOOST_PROCESS_V2_WINDOWS)
std::vector<pid_type> all_pids(error_code & ec)
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
    if (Process32First(hp, &pe)) {
      do {
        vec.push_back(pe.th32ProcessID);
      } while (Process32Next(hp, &pe));
    }
    CloseHandle(hp);
    return vec;
}

#elif (defined(__APPLE__) && defined(__MACH__))

std::vector<pid_type> all_pids(error_code & ec)
{
    std::vector<pid_type> vec;
    vec.resize(proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0));

    if (proc_listpids(PROC_ALL_PIDS, 0, &vec[0], sizeof(pid_type) * vec))
    {
        ec = detail::get_last_error();
        return {};
    }

    auto itr = std::partition(vec.begin(), vec.end(), [](pid_type pt) {return pt != 0;});
    vec.erase(itr, vec.end());
    return vec;
}

#elif defined(__linux__)

std::vector<pid_type> all_pids(error_code & ec)
{
    std::vector<pid_type> vec;
    DIR *proc = opendir("/proc");
    if (proc == nullptr)
    {
        ec = detail::get_last_error();
        return vec;
    } 
    struct dirent *ent = nullptr;
    while ((ent = readdir(proc) != nullptr)) 
    {
      if (!isdigit(*ent->d_name))
        continue;
      vec.push_back(atoi(ent->d_name));
    }
    closedir(proc);
    return vec;
}

#elif defined(__FreeBSD__)

std::vector<pid_type> all_pids(error_code & ec)
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
      for (int i = 0; i < cntp; i++) {
        if (proc_info[i].kp_pid >= 0) {
          vec.push_back(proc_info[i].kp_pid);
        }
      }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

#elif defined(__NetBSD__)

std::vector<pid_type> all_pids(error_code & ec)
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
      for (int i = cntp - 1; i >= 0; i--) {
        vec.push_back(proc_info[i].p_pid);
      }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

#elif defined(__OpenBSD__)

std::vector<pid_type> all_pids(error_code & ec)
{
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
      vec.reserve(cntp);
      for (int i = 0; i < cntp; i++) {
        if (proc_info[i].kp_pid >= 0) {
          vec.push_back(proc_info[i].kp_pid);
        }
      }
    }
    else
        ec = detail::get_last_error();
    kvm_close(kd);
    return vec;
}

#elif defined(__sun)

std::vector<pid_type> all_pids(error_code & ec)
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
      if (kvm_kread(kd, (std::uintptr_t)proc_info->p_pidp, &cur_pid, sizeof(cur_pid)) != -1) {
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


#else
#error "Platform not supported"


#endif defined(BOOST_PROCESS_V2_WINDOWS)


std::vector<pid_type> all_pids()
{
    error_code ec;
    auto res = all_pids(ec);
    if (ec)
        detail::throw_error(ec, "all_pids");
    return res;
}



BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_IMPL_PID_IPP
