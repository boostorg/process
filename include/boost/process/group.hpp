// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/group.hpp
 *
 * Defines a group process class.
 */

#ifndef BOOST_PROCESS_GROUP_HPP
#define BOOST_PROCESS_GROUP_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/child.hpp>
#include <chrono>
#include <memory>

#include <boost/none.hpp>
#include <atomic>


#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/group_handle.hpp>
#include <boost/process/detail/posix/group_ref.hpp>
#include <boost/process/detail/posix/wait_group.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/group_handle.hpp>
#include <boost/process/detail/windows/group_ref.hpp>
#include <boost/process/detail/windows/wait_group.hpp>
#endif

namespace boost {

namespace process {

namespace detail {
    struct group_builder;
}

/**
 * Represents a group process.
 *
 * On Windows group is movable but non-copyable. The destructor
 * automatically closes handles to the group process.
 */
class group
{
    ::boost::process::detail::api::group_handle _group_handle;
    bool _attached = true;
public:
    typedef ::boost::process::detail::api::group_handle group_handle;
    typedef group_handle::handle_t native_handle_t;
    explicit group(group_handle &&ch) : _group_handle(std::move(ch)) {}
    explicit group(native_handle_t & handle) : _group_handle(handle) {};
    group(const group&) = delete;
    group(group && lhs)
        : _group_handle(std::move(lhs._group_handle)),
          _attached (lhs._attached)
    {
        lhs._attached = false;
    }

    group() = default;
    group& operator=(const group&) = delete;
    group& operator=(group && lhs)
    {
        _group_handle= std::move(lhs._group_handle);
        _attached    = lhs._attached;

        return *this;
    };

    void detach() {_attached = false; }
    void attach() {_attached = true;}

    ~group()
    {
        if ( _attached && valid())
            terminate();
    }
    native_handle_t native_handle() const { return _group_handle.handle(); }


    void wait()
    {
        boost::process::detail::api::wait(_group_handle);
    }

    template< class Rep, class Period >
    bool wait_for  (const std::chrono::duration<Rep, Period>& rel_time)
    {
        return boost::process::detail::api::wait_for(_group_handle, rel_time);
    }

    template< class Clock, class Duration >
    bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time )
    {
        return boost::process::detail::api::wait_until(_group_handle, timeout_time);
    }

    bool valid() const
    {
        return _group_handle.valid();
    }
    operator bool() const {return valid();}

    void terminate()
    {
        ::boost::process::detail::api::terminate(_group_handle);
    }

    void assign(const child &c)
    {
        _group_handle.assign(c.native_handle());
    }
    void assign(const child &c, std::error_code & ec)
    {
        _group_handle.assign(c.native_handle(), ec);
    }

    bool has(const child &c)
    {
        return _group_handle.has(c.native_handle());
    }
    bool has(const child &c, std::error_code & ec)
    {
        return _group_handle.has(c.native_handle(), ec);

    }

    friend struct detail::group_builder;
};

//for now it'll just be declared here.
namespace detail
{

struct group_tag;
struct group_builder
{
    group * group_p;

    void operator()(group & grp) {this->group_p = &grp;};

    typedef api::group_ref result_type;
    api::group_ref get_initializer() {return api::group_ref (group_p->_group_handle);};
};

template<>
struct initializer_tag<group>
{
    typedef group_tag type;
};

template<>
struct initializer_builder<group_tag>
{
    typedef group_builder type;
};

}
}}
#endif

