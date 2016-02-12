// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ENV_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ENV_HPP

#include <boost/detail/winapi/process.hpp>
#include <boost/process/detail/initializers/base.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/shared_array.hpp>
#include <iterator>
#include <cstddef>

namespace boost { namespace process { namespace windows { namespace initializers {

template <class Range>
struct set_env_t : ::boost::process::detail::initializers::base
{
    typedef typename Range::value_type string_type;
    typedef typename string_type::value_type char_type;
private:

    static std::size_t add_size(std::size_t size, const string_type &s)
    {
        return size + s.size() + 1u;
    }

    struct copy
    {
        char_type *it_;

        copy(char_type *it) : it_(it) {}

        void operator()(const string_type &s)
        {
            it_ = boost::copy(s, it_);
            *it_ = 0;
            ++it_;
        }
    };

    static BOOST_CONSTEXPR ::boost::detail::winapi::DWORD_ unicode_flag(wchar_t)
    {
        return ::boost::detail::winapi::CREATE_UNICODE_ENVIRONMENT_;
    }

    static BOOST_CONSTEXPR ::boost::detail::winapi::DWORD_ unicode_flag(char)
    {
        return static_cast<::boost::detail::winapi::DWORD_>(0);
    }

public:
    set_env_t(const Range &envs)
        : size_(boost::accumulate(envs, 0, add_size) + 1),
        env_(new char_type[size_])
    {
        boost::for_each(envs, copy(env_.get()));
        env_[size_ - 1] = 0;
    }


    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.env = env_.get();

        e.creation_flags |= unicode_flag(char_type());
    }

private:
    std::size_t size_;
    boost::shared_array<char_type> env_;
};

struct set_env_
{
    template <class Range>
    set_env_t<Range> operator()(const Range &envs) const
    {
        return set_env_t<Range>(envs);
    }
    template <class Range>
    set_env_t<Range> operator= (const Range &envs) const
    {
        return set_env_t<Range>(envs);
    }
};

static constexpr set_env_ set_env;

}}}}

#endif
