// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_ERROR_HPP
#define BOOST_PROCESS_DETAIL_ERROR_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/system/error_code.hpp>

namespace boost { namespace process {

namespace detail {

struct set_on_error : ::boost::process::detail::handler
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

struct error_
{
    set_on_error operator()(boost::system::error_code &ec) const {return set_on_error(ec);}
    set_on_error operator= (boost::system::error_code &ec) const {return set_on_error(ec);}

};

}

constexpr static boost::process::detail::error_ error;
constexpr static boost::process::detail::error_ error_ref;
constexpr static boost::process::detail::error_ error_code;


}}

#endif
