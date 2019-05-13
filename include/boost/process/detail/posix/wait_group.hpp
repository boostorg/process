// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_WAIT_GROUP_HPP
#define BOOST_PROCESS_DETAIL_POSIX_WAIT_GROUP_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/posix/group_handle.hpp>
#include <chrono>
#include <system_error>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace boost { namespace process { namespace detail { namespace posix {

inline void wait(const group_handle &p, std::error_code &ec) noexcept
{
    pid_t ret;
    siginfo_t  status;

    do
    {
        ret = ::waitpid(-p.grp, &status.si_status, 0);
        if (ret == -1)
        {
            ec = get_last_error();
            return; 
        }

        //ECHILD --> no child processes left.
        ret = ::waitid(P_PGID, p.grp, &status, WEXITED | WNOHANG);
    } 
    while ((ret != -1) || (errno != ECHILD));
   
    if (errno != ECHILD)
        ec = boost::process::detail::get_last_error();
    else
        ec.clear();
}

inline void wait(const group_handle &p) noexcept
{
    std::error_code ec;
    wait(p, ec);
    boost::process::detail::throw_error(ec, "waitpid(2) failed in wait");
}

template< class Clock, class Duration >
inline bool wait_until(
        const group_handle &p,
        const std::chrono::time_point<Clock, Duration>& time_out,
        std::error_code & ec) noexcept
{

    ::siginfo_t siginfo;


    auto get_timespec = 
            +[](const Duration & dur)
            {
                ::timespec ts;
                ts.tv_sec  = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
                ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() % 1000000000;
                return ts;
            };


    bool timed_out = false;
    int ret;

#if defined(BOOST_POSIX_HAS_SIGTIMEDWAIT)

    ::sigset_t  sigset;

    if (sigemptyset(&sigset) != 0)
    {
        ec = get_last_error();
        return false;
    }
    if (sigaddset(&sigset, SIGCHLD) != 0)
    {
        ec = get_last_error();
        return false;
    }

    struct ::sigaction old_sig;
    if (-1 == ::sigaction(SIGCHLD, nullptr, &old_sig))
    {
        ec = get_last_error();
        return false;
    }

    do
    {
        auto ts = get_timespec(time_out - Clock::now());
        ret = ::sigtimedwait(&sigset, nullptr, &ts);
        errno = 0;
        if ((ret == SIGCHLD) && (old_sig.sa_handler != SIG_DFL) && (old_sig.sa_handler != SIG_IGN))
            old_sig.sa_handler(ret);

        ret = ::waitpid(-p.grp, &siginfo.si_status, 0); //so in case it exited, we wanna reap it first
        if (ret == -1)
        {
            ec = get_last_error();
            return false; 
        }

        //check if we're done ->
        ret = ::waitid(P_PGID, p.grp, &siginfo, WEXITED | WNOHANG);
    }
    while (((ret != -1) || ((errno != ECHILD) && (errno != ESRCH))) && !(timed_out = (Clock::now() > time_out)));

    if (errno != ECHILD)
    {
        ec = boost::process::detail::get_last_error();
        return !timed_out;
    }
    else
    {
        ec.clear();
        return true; //even if timed out, there are no child proccessess left
    }

#else
    //if we do not have sigtimedwait, we fork off a child process  to get the signal in time
	int p_[2];
	::pipe(p_);

    pid_t timeout_pid = ::fork();

    if (timeout_pid == -1)
    {
        ec = boost::process::detail::get_last_error();
        return true;
    }
    else if (timeout_pid == 0)
    {
		::setpgid(0, p.grp);
		static ::gid_t gid = 0;
		gid = p.grp;

		
		::signal(SIGUSR1, +[](int){::setpgid(0, 0);});
		::signal(SIGUSR2, +[](int){::setpgid(0, gid);});

		::close(p_[0]);
		::write(p_[1], &p_[0], sizeof(int));
		::close(p_[1]);
		
        auto ts = get_timespec(time_out - Clock::now());
        ::timespec rem;
        while (ts.tv_sec > 0 || ts.tv_nsec > 0)
        {
            if (::nanosleep(&ts, &rem) != 0)
            {
                auto err = errno;
                if ((err == EINVAL) || (err == EFAULT))
                    break;
            }
            ts = get_timespec(time_out - Clock::now());
        }
        ::exit(0);
    }

    struct child_cleaner_t
    {
        pid_t pid;
        ~child_cleaner_t()
        {
            int res;
            ::kill(pid, SIGKILL);
            ::waitpid(pid, &res, 0);
        }
    };
    child_cleaner_t child_cleaner{timeout_pid};

	::close(p_[1]);
	::read (p_[0], &p_[1], sizeof(int));
	::close(p_[0]);

    while (!(timed_out = (Clock::now() > time_out)))
    {
        ret = ::waitid(P_PGID, p.grp, &siginfo, WEXITED | WSTOPPED);
        if (ret == -1)
        {
            if ((errno == ECHILD) || (errno == ESRCH))
            {
                ec.clear();
                return true;
            }
            ec = boost::process::detail::get_last_error();
            return false;
        }

        ::kill(timeout_pid, SIGUSR1);
        //if it is not, we gotta check who the signal came from, so let's remove the process from the group
        //check if the group is empty -> will give an error of ECHILD
        ret = ::waitid(P_PGID, p.grp, &siginfo, WEXITED | WCONTINUED | WNOWAIT | WNOHANG);

        if (ret == -1)
        {
            if ((errno == ECHILD) || (errno == ESRCH))
            {
                ec.clear();
                return true;
            }
            else
            {
                ec = boost::process::detail::get_last_error();
                return false;
            }
        }
        else
            ::kill(timeout_pid, SIGUSR2);
    }
    return !timed_out;
#endif
}

template< class Clock, class Duration >
inline bool wait_until(
        const group_handle &p,
        const std::chrono::time_point<Clock, Duration>& time_out) noexcept
{
    std::error_code ec;
    bool b = wait_until(p, time_out, ec);
    boost::process::detail::throw_error(ec, "waitpid(2) failed in wait_until");
    return b;
}

template< class Rep, class Period >
inline bool wait_for(
        const group_handle &p,
        const std::chrono::duration<Rep, Period>& rel_time,
        std::error_code & ec) noexcept
{
    return wait_until(p, std::chrono::steady_clock::now() + rel_time, ec);
}

template< class Rep, class Period >
inline bool wait_for(
        const group_handle &p,
        const std::chrono::duration<Rep, Period>& rel_time) noexcept
{
    std::error_code ec;
    bool b = wait_for(p, rel_time, ec);
    boost::process::detail::throw_error(ec, "waitpid(2) failed in wait_for");
    return b;
}

}}}}

#endif
