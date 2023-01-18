// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_EXT_IMPL_ENV_IPP
#define BOOST_PROCESS_V2_EXT_IMPL_ENV_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/ext/detail/proc_info.hpp>
#include <boost/process/v2/ext/env.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <shellapi.h>
#else
#include <cstdlib>
#endif

#if (defined(__linux__) || defined(__ANDROID__))
#include <cstdio>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail {
namespace ext {

#if (defined(__linux__) || defined(__ANDROID__))
//linux stores this as a blob with an EOF at the end

void native_env_handle_deleter::operator()(native_env_handle_type h) const
{
    delete [] h;
}

native_env_iterator next(native_env_iterator nh)
{
    while (*nh != '\0')
        nh ++;
    return ++nh ;
}
native_env_iterator find_end(native_env_iterator nh)
{
    while (*nh != EOF)
        nh ++;
    return nh ;
}

const environment::char_type * dereference(native_env_iterator iterator)
{
    return iterator;
}

#endif

}
}

namespace ext
{

#if defined(_WIN32)

env_view env(HANDLE proc, boost::system::error_code & ec)
{
    wchar_t *buffer = nullptr;
    PEB peb;
    SIZE_T nRead = 0; 
    ULONG len = 0;
    PROCESS_BASIC_INFORMATION pbi;
    detail::ext::RTL_USER_PROCESS_PARAMETERS_EXTENDED upp;

    NTSTATUS status = 0;
    PVOID buf = nullptr;
    status = NtQueryInformationProcess(proc, ProcessBasicInformation, &pbi, sizeof(pbi), &len);
    ULONG error = RtlNtStatusToDosError(status);

    if (error)
    {
        ec.assign(error, boost::system::system_category());
        return {};
    }

    if (!ReadProcessMemory(proc, pbi.PebBaseAddress, &peb, sizeof(peb), &nRead))
    {
        ec = detail::get_last_error();
        return {};
    }

    if (!ReadProcessMemory(proc, peb.ProcessParameters, &upp, sizeof(upp), &nRead))
    {
        ec = detail::get_last_error();
        return {};
    }

    env_view ev;
    buf = upp.Environment;
    len = (ULONG)upp.EnvironmentSize;
    ev.handle_.reset(new wchar_t[len / 2 + 1]());

    if (!ReadProcessMemory(proc, buf, ev.handle_.get(), len, &nRead))
    {
        ec = detail::get_last_error();
        return {};
    }

    ev.handle_.get()[len / 2] = L'\0';
    return ev;
}

env_view env(boost::process::v2::pid_type pid, boost::system::error_code & ec)
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
	    return env(proc.get(), ec);

	return {};
}


#elif (defined(__linux__) || defined(__ANDROID__))

env_view env(boost::process::v2::pid_type pid, boost::system::error_code & ec)
{
    std::size_t size = 0;
    std::unique_ptr<char, detail::ext::native_env_handle_deleter> procargs{};

    int f = ::open(("/proc/" + std::to_string(pid) + "/environ").c_str(), O_RDONLY);

    while (!procargs || procargs.get()[size - 1] != EOF)
    {
        std::unique_ptr<char, detail::ext::native_env_handle_deleter> buf{new char[size + 4096]};
        if (size > 0)
            std::memmove(buf.get(), procargs.get(), size);
        auto r = ::read(f, buf.get() + size, 4096);
        if (r < 0)
        {
            ec = detail::get_last_error();
            ::close(f);
            return {};
        }
        procargs = std::move(buf);
        size += r;
        if (r < 4096) // done!
        {
            procargs.get()[size] = EOF;
            break;
        }
    }
    ::close(f);

    env_view ev;
    ev.handle_ = std::move(procargs);
    return ev;
}

#endif

env_view env(boost::process::v2::pid_type pid)
{
    boost::system::error_code ec;
    auto res = env(pid, ec);
    if (ec)
        detail::throw_error(ec, "env");
    return res;
}
}
BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_EXT_IMPL_ENV_IPP
