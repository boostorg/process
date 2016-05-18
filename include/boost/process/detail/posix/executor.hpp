// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_EXECUTOR_HPP
#define BOOST_PROCESS_POSIX_EXECUTOR_HPP

#include <boost/process/child.hpp>
#include <boost/process/error.hpp>
#include <boost/process/detail/posix/pipe.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <cstdlib>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sysexits.h>
#include <unistd.h>


namespace boost { namespace process { namespace detail { namespace posix {

template<typename Executor>
struct on_setup_t
{
    Executor & exec;
    on_setup_t(Executor & exec) : exec(exec) {};
    template<typename T>
    void operator()(T & t) const {t.on_setup(exec);}
};

template<typename Executor>
struct on_error_t
{
    Executor & exec;
    const std::error_code & error;
    on_error_t(Executor & exec, const std::error_code & error) : exec(exec), error(error) {};
    template<typename T>
    void operator()(T & t) const {t.on_error(exec, error);}
};

template<typename Executor>
struct on_success_t
{
    Executor & exec;
    on_success_t(Executor & exec) : exec(exec) {};
    template<typename T>
    void operator()(T & t) const {t.on_success(exec);}
};



template<typename Executor>
struct on_fork_error_t
{
    Executor & exec;
    const std::error_code & error;
    on_fork_error_t(Executor & exec, const std::error_code & error) : exec(exec), error(error) {};
    template<typename T>
    void operator()(T & t) const {t.on_fork_error(exec, error);}
};


template<typename Executor>
struct on_exec_setup_t
{
    Executor & exec;
    on_exec_setup_t(Executor & exec) : exec(exec) {};
    template<typename T>
    void operator()(T & t) const {t.on_exec_setup(exec);}
};


template<typename Executor>
struct on_exec_error_t
{
    Executor & exec;
    const std::error_code &ec;
    on_exec_error_t(Executor & exec, const std::error_code & error) : exec(exec), ec(error) {};
    template<typename T>
    void operator()(T & t) const {t.on_exec_error(exec, ec);}
};

template<typename Executor> on_setup_t  <Executor> call_on_setup  (Executor & exec) {return exec;}
template<typename Executor> on_error_t  <Executor> call_on_error  (Executor & exec, const std::error_code & ec) 
{
    return on_error_t<Executor> (exec, ec);
}
template<typename Executor> on_success_t<Executor> call_on_success(Executor & exec) {return exec;}

template<typename Executor> on_fork_error_t  <Executor> call_on_fork_error  (Executor & exec, const std::error_code & ec) 
{
    return on_fork_error_t<Executor> (exec, ec);
}

template<typename Executor> on_exec_setup_t  <Executor> call_on_exec_setup  (Executor & exec) {return exec;}
template<typename Executor> on_exec_error_t  <Executor> call_on_exec_error  (Executor & exec, const std::error_code & ec) 
{
    return on_exec_error_t<Executor> (exec, ec);
}


template<typename Sequence>
struct executor
{
    executor(Sequence & seq) : seq(seq)
    {
    }

    void internal_throw(boost::mpl::true_, std::error_code &ec ) {}
    void internal_throw(boost::mpl::false_, std::error_code &ec ) {throw std::system_error(ec);}

    typedef typename ::boost::process::detail::has_error_handler<Sequence>::type has_error_handler;
    typedef typename has_ignore_error<Sequence>::type ignore_error;


    void invoke(boost::mpl::true_) //ignore errors
    {
        boost::fusion::for_each(seq, call_on_setup(*this));

        pid_t pid = ::fork();
        if (pid == -1)
        {
            auto ec = boost::process::detail::get_last_error();
            boost::fusion::for_each(seq, call_on_fork_error(*this, ec));
        }
        else if (pid == 0)
        {
            boost::fusion::for_each(seq, call_on_exec_setup(*this));
            ::execve(exe, cmd_line, env);
            auto ec = boost::process::detail::get_last_error();
            boost::fusion::for_each(seq, call_on_exec_error(*this, ec));
            _exit(EXIT_FAILURE);
        }

        boost::fusion::for_each(seq, call_on_success(*this));
        this->pid = pid;

        return child(pid);
    }
    void invoke(boost::mpl::false_)
    {
        pipe p = pipe::create();


        if (::fcntl(p.sink(), F_SETFD, FD_CLOEXEC) == -1)
            boost::process::detail::throw_last_error("fcntl(2) failed");

        boost::fusion::for_each(seq, call_on_setup(*this));

        pid_t pid = ::fork();
        if (pid == -1)
        {
            auto ec = boost::process::detail::get_last_error();
            boost::fusion::for_each(seq, call_on_fork_error(*this, ec));
        }
        else if (pid == 0)
        {
            ::close(p.source());
            boost::fusion::for_each(seq, call_on_exec_setup(*this));

            ::execve(exe, cmd_line, env);
            std::error_code ec = boost::process::detail::get_last_error();
            boost::fusion::for_each(seq, call_on_exec_error(*this, ec));
            auto err = ec.value();

            while (::write(p.sink(), &err, sizeof(int)) == -1 && errno == EINTR);
            ::close(p.sink());
            _exit(EXIT_FAILURE);
        }

        int exec_err;
        int count;
        ::close(p.sink());

        do
        {
            //actually, this should block until it's read.
            count = ::read(p.source(), &exec_err, sizeof(errno));
            auto err = errno;
            if (err == EBADF)//that should occur on success.
                break;
            //EAGAIN not yet forked, EINTR interrupted, i.e. try again
            else if ((err != EAGAIN ) && (err != EINTR)) {}
            else //other error.
                boost::process::detail::throw_last_error("Error read pipe");
        }
        while (count == -1); //wait until i can read

        if (count == 4)
        {
            //error
            std::error_code ec(exec_err, std::system_category());
            boost::fusion::for_each(seq, call_on_error(*this, ec));

            internal_throw(has_error_handler(), ec);

        }
        else if (count > 0)
            boost::process::detail::throw_last_error("Error reading the error pipe");
        else
        {
            this->pid = pid;
            boost::fusion::for_each(seq, call_on_success(*this));
        }

    }

    child operator()()
    {
        invoke(ignore_error());

        return child(child_handle(pid));
    }

    Sequence & seq;
    const char * exe      = nullptr;
    char *const* cmd_line = nullptr;
    char **env      = nullptr;
    pid_t pid = -1;
    
};


template<typename Tup>
executor<Tup> make_executor(Tup & tup)
{
    return executor<Tup>(tup);
}

}}}}

#endif
