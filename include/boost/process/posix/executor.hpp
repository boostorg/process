// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_EXECUTOR_HPP
#define BOOST_PROCESS_POSIX_EXECUTOR_HPP

#include <boost/process/posix/child.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <cstdlib>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sysexits.h>
#include <unistd.h>

namespace boost { namespace process { namespace posix {

struct executor
{
    executor() : exe(0), cmd_line(0), env(0), pid(-1), error_pipe(create_pipe(FD_CLOEXEC, 0))
    {

    }

    struct call_on_fork_setup
    {
        executor &e_;

        call_on_fork_setup(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(const Arg &arg) const
        {
            arg.on_fork_setup(e_);
        }
    };

    struct call_on_fork_error
    {
        executor &e_;

        call_on_fork_error(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_fork_error(e_);
        }
    };

    struct call_on_fork_success
    {
        executor &e_;

        call_on_fork_success(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_fork_success(e_);
        }
    };

    struct call_on_exec_setup
    {
        executor &e_;

        call_on_exec_setup(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_exec_setup(e_);
        }
    };

    struct call_on_exec_error
    {
        executor &e_;

        call_on_exec_error(executor &e) : e_(e) {}

        template <class Arg>
        void operator()(Arg &arg) const
        {
            arg.on_exec_error(e_);
        }
    };

    template <class InitializerSequence>
    child operator()(const InitializerSequence &seq)
    {
        boost::fusion::for_each(seq, call_on_fork_setup(*this));
        //set up the pipe for error handling.
        pid = ::fork();
        if (pid == -1)
        {
            boost::fusion::for_each(seq, call_on_fork_error(*this));
        }
        else if (pid == 0)
        {
            ::close(error_pipe.source);//doesn't read
            boost::fusion::for_each(seq, call_on_exec_setup(*this));
            ::execve(exe, cmd_line, env);
            boost::fusion::for_each(seq, call_on_exec_error(*this));
            ::write(error_pipe.sink, &errno, sizeof(int));
            ::_exit(EXIT_FAILURE);
        }

        int exec_err;
        int count;
        do
        {
            //actually, this should block until it's read.
            count = ::read(error_pipe.source, &exec_err, sizeof(errno));
            auto err = errno;
            if (err == EBADF)//that should occur on success.
                break;
            //EAGAIN not yet forked, EINTR interrupted, i.e. try again
            else if ((err != EAGAIN ) && (errno != EINTR)) {}
            else //other error.
            {
                BOOST_PROCESS_THROW_LAST_SYSTEM_ERROR("Error read pipe");
            }
        }
        while (count == -1); //wait until i can read

        if (count == 4)
        {
            //error
        }
        else if (count > 0) //invalid read
        {

        }
        else
        {
            //launch succeeded.
        }
        boost::fusion::for_each(seq, call_on_fork_success(*this));

        return child(pid);
    }

    const char *exe;
    char **cmd_line;
    char **env;
    pid_t pid;
private:
    pipe error_pipe;
};

}}}

#endif
