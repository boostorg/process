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

#include <boost/process/config.hpp>
#include <chrono>
#include <memory>

#include <boost/none.hpp>


#include <boost/hana/type.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/unpack.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix/child_handle.hpp>
#include <boost/process/posix/wait_for_exit.hpp>
#include <boost/process/posix/is_running.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/windows/child_handle.hpp>
#include <boost/process/windows/wait_for_exit.hpp>
#include <boost/process/windows/is_running.hpp>

#endif
namespace boost {

namespace process {

namespace detail {

template<typename ...Args>
struct resource_t : Args...
{
    resource_t(const resource_t &) = delete;
    resource_t(resource_t &&) = default;
    resource_t(Args&&...args) : Args(args)... {}
};

template<typename ...Args>
auto make_resource(const boost::hana::tuple<Args...> & tup)
{
    //ok, get the resource types, already initialized.
    auto res = boost::hana::transform(tup, [](const auto & v) {return initializer_resource(v);});

    auto filtered = boost::hana::filter(res, [](auto v){
                        return boost::hana::decltype_(v)
                            == boost::hana::decltype_(boost::none);
                    });

    auto maker = [](auto &&... args)
    {
        return resource_t<decltype(args)...>(std::forward<decltype(args)>(args)...);
    };

    return boost::hana::unpack(filtered, maker);
}

template<typename ...Args>
std::unique_ptr<void, void(*)(void*)> make_resource_pointer(const boost::hana::tuple<Args...> & tup)
{
    auto r = make_resource(tup);
    typedef decltype(r) r_t;
    //safe construction of the ptr.
    auto up = std::make_unique<r_t>(std::move(r));

    auto p = up.release();

    auto del = +[](r_t * p) {delete p;};

    typedef void(*f_p)(void*);

    return std::unique_ptr<void, f_p>(
            reinterpret_cast<void*>(p),
            reinterpret_cast<f_p>(del));
}

}

/**
 * Represents a child process.
 *
 * On Windows child is movable but non-copyable. The destructor
 * automatically closes handles to the child process.
 */
class child
{
    ::boost::process::detail::api::child_handle _child_handle;
    std::unique_ptr<void, void(*)(void*)> _resource;
    int _exit_code = -1;
    bool _attached = true;
    bool _exited = false;
public:
    typedef std::unique_ptr<void, void(*)(void*)> resource_t;
    typedef ::boost::process::detail::api::child_handle child_handle;
    typedef child_handle::process_handle_t native_handle_t;
    explicit child(child_handle &&ch, resource_t && resource) : _child_handle(std::move(ch)), _resource(std::move(resource)) {}

    child(const child&) = delete;
    child(child && ) = default;

    child& operator=(const child&) = delete;
    child& operator=(child && ) = default;

    native_handle_t native_handle() const { return _child_handle.process_handle(); }


    int exit_code() const;
    int get_pid() const;

    bool running();

    void terminate();

    void wait()
    {
        boost::process::detail::api::wait(_child_handle, _exit_code);
        _exited = !boost::process::detail::api::is_running(_child_handle);
    }

    template< class Rep, class Period >
    bool wait_for  (const std::chrono::duration<Rep, Period>& rel_time)
    {
        auto b = boost::process::detail::api::wait_for(_child_handle, _exit_code, rel_time);
        _exited = !boost::process::detail::api::is_running(_child_handle);
        return b;
    }

    template< class Clock, class Duration >
    bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time )
    {
        auto b = boost::process::detail::api::wait_until(_child_handle, _exit_code, timeout_time);
        _exited = !boost::process::detail::api::is_running(_child_handle);
        return b;
    }

    bool valid() const
    {
        return _child_handle.valid();
    }


    void detach();
    void attach();

};

}}
#endif

