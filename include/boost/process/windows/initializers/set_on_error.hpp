// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ON_ERROR_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ON_ERROR_HPP

#include <boost/process/config.hpp>
#include <boost/process/detail/initializers/base.hpp>
#include <boost/system/error_code.hpp>

namespace boost { namespace process { namespace windows { namespace initializers {

struct set_on_error : ::boost::process::detail::initializers::base
{
    set_on_error(const set_on_error&) = default;
    explicit set_on_error(boost::system::error_code &ec) : ec_(ec) {}

    template <class WindowsExecutor>
    void on_error(WindowsExecutor&, const boost::system::error_code & ec) const
    {
        ec_ = ec;
    }

private:
    boost::system::error_code &ec_;
};

struct error_ref_
{
    set_on_error operator()(boost::system::error_code &ec) const {return set_on_error(ec);}
    set_on_error operator= (boost::system::error_code &ec) const {return set_on_error(ec);}

};

constexpr error_ref_ error_ref;


}}}}

#endif
