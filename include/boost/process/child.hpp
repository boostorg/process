// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/child.hpp
 *
 * Defines a child process class.
 */

#ifndef BOOST_PROCESS_CHILD_HPP
#define BOOST_PROCESS_CHILD_HPP

#include <boost/process/detail/config.hpp>
#include <chrono>
#include <memory>

#include <boost/none.hpp>
#include <atomic>


#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/child_handle.hpp>
#include <boost/process/detail/posix/terminate.hpp>
#include <boost/process/detail/posix/wait_for_exit.hpp>
#include <boost/process/detail/posix/is_running.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/child_handle.hpp>
#include <boost/process/detail/windows/terminate.hpp>
#include <boost/process/detail/windows/wait_for_exit.hpp>
#include <boost/process/detail/windows/is_running.hpp>

#endif
namespace boost {

namespace process {

using ::boost::process::detail::api::pid_t;

/**
 * Represents a child process.
 *
 * On Windows child is movable but non-copyable. The destructor
 * automatically closes handles to the child process.
 */
class child
{
    ::boost::process::detail::api::child_handle _child_handle;
    std::atomic<bool> _exited{ false };
    std::atomic<int> _exit_code{ -1 };
    bool _attached = true;
public:
    typedef ::boost::process::detail::api::child_handle child_handle;
    typedef child_handle::process_handle_t native_handle_t;
    explicit child(child_handle &&ch) : _child_handle(std::move(ch)) {}
    explicit child(pid_t & pid) : _child_handle(pid) {};
    child(const child&) = delete;
    child(child && lhs)
        : _child_handle(std::move(lhs._child_handle)),
          _exited   (lhs._exited.load()),
          _exit_code(lhs._exit_code.load()),
          _attached (lhs._attached)
    {
        lhs._attached = false;
    }

    child() = default;
    child& operator=(const child&) = delete;
    child& operator=(child && lhs)
    {
        _child_handle= std::move(lhs._child_handle);
        _exited   .store(lhs._exited.load()   );
        _exit_code.store(lhs._exit_code.load());
        _attached    = lhs._attached;
        lhs._attached = false;
        return *this;
    };

    void detach() {_attached = false; }

    ~child()
    {
        if (!_exited.load() && _attached && running())
            terminate();
    }
    native_handle_t native_handle() const { return _child_handle.process_handle(); }


    int exit_code() const {return _exit_code.load();}
    pid_t id()      const {return _child_handle.id(); }

    bool running()
    {
        if (valid() && !_exited.load())
        {
            int code; 
            auto res = boost::process::detail::api::is_running(_child_handle, code);
            if (!res && !_exited.load())
            {
                _exited.store(true);
                _exit_code.store(code);
            }
            return res;
        }
        return false;
    }

    void terminate()
    {
        if (valid())
            boost::process::detail::api::terminate(_child_handle);

        _exited.store(true);
    }

    void wait()
    {
        if (!_exited.load() && valid())
        {
            int exit_code = 0;
            boost::process::detail::api::wait(_child_handle, exit_code);
            _exited.store(true);
            _exit_code.store(exit_code);
        }
    }

    template< class Rep, class Period >
    bool wait_for  (const std::chrono::duration<Rep, Period>& rel_time)
    {
        if (!_exited.load())
        {
            int exit_code = 0;
            auto b = boost::process::detail::api::wait_for(_child_handle, _exit_code, rel_time);
            _exited.store(b);
            if (!b)
                return false;
            _exit_code.store(exit_code);
        }
        return true;
    }

    template< class Clock, class Duration >
    bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time )
    {
        if (!_exited.load())
        {
            int exit_code = 0;
            auto b = boost::process::detail::api::wait_until(_child_handle, _exit_code, timeout_time);
            _exited.store(b);
            if (!b)
                return false;
            _exit_code.store(exit_code);
        }
        return true;
    }

    bool valid() const
    {
        return _child_handle.valid();
    }
    operator bool() const {return valid();}

    void set_exited(int code)
    {
        _exit_code.store(code);
        _exited.store(true);
    }

    bool in_group() const
    {
        return _child_handle.in_group();
    }
    bool in_group(std::error_code &ec) const
    {
        return _child_handle.in_group(ec);
    }
};

}}
#endif

