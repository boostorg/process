// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_START_IN_DIR_HPP
#define BOOST_PROCESS_START_IN_DIR_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/handler.hpp>

#if defined (BOOST_POSIX_API)
#include <boost/process/detail/posix/initializers.hpp>
#elif defined (BOOST_WINDOWS_API)
#include <boost/process/detail/windows/start_dir.hpp>
#endif

#include <boost/process/detail/config.hpp>
#include <string>
#include <boost/filesystem/path.hpp>

namespace boost { namespace process { namespace detail {

class start_dir_init : public handler
{
public:
    explicit start_dir_init(const std::string &s) : s_(s) {}

    template <class Executor>
    void on_setup(Executor& exec) const
    {
        api::apply_start_dir(exec, s_);
    }

private:
    std::string s_;
};

struct start_dir_
{
    constexpr start_dir_() {};

    start_dir_init operator()(const std::string & st)             const {return start_dir_init(st); }
    start_dir_init operator()(std::string && s)                   const {return start_dir_init(std::move(s)); }
    start_dir_init operator()(const boost::filesystem::path & st) const {return start_dir_init(st.string()); }

    start_dir_init operator=(const std::string & st)             const {return start_dir_init(st); }
    start_dir_init operator=(std::string && s)                   const {return start_dir_init(std::move(s)); }
    start_dir_init operator=(const boost::filesystem::path & st) const {return start_dir_init(st.string()); }

};

constexpr static start_dir_ start_dir;

}
using ::boost::process::detail::start_dir;
}}

#endif
