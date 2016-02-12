// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_SHARED_INITIALIZER_BASE_HPP
#define BOOST_PROCESS_DETAIL_SHARED_INITIALIZER_BASE_HPP

#include <boost/system/system_error.hpp>

namespace boost { namespace process { namespace detail { namespace initializers {

template<template <typename> class Template>
struct make_handler_t
{
    template<typename Handler>
    constexpr Template<Handler> operator()(Handler handler) const {return Template<Handler>(handler);}
    template<typename Handler>
    constexpr Template<Handler> operator= (Handler handler) const {return Template<Handler>(handler);}
    template<typename Handler>
    constexpr Template<Handler> operator+=(Handler handler) const {return Template<Handler>(handler);}
};

struct base
{
    template <class Executor>
    void on_setup(Executor&) const {}

    template <class Executor>
    void on_error(Executor&, const boost::system::error_code &ec) const {}

    template <class Executor>
    void on_success(Executor&) const {}

};

template <class Handler>
class on_setup_ : public base
{
public:
    explicit on_setup_(Handler handler) : handler_(handler) {}

    template <class Executor>
    void on_setup(Executor &e) const
    {
        handler_(e);
    }
private:
    Handler handler_;
};

template <class Handler>
class on_error_ : public base
{
public:
    explicit on_error_(Handler handler) : handler_(handler) {}

    template <class Executor>
    void on_error(Executor &e, const boost::system::error_code &ec) const
    {
        handler_(e);
    }
private:
    Handler handler_;
};

template <class Handler>
class on_success_ : public base
{
public:
    explicit on_success_(Handler handler) : handler_(handler) {}

    template <class Executor>
    void on_success(Executor &e) const
    {
        handler_(e);
    }
private:
    Handler handler_;
};

constexpr make_handler_t<on_setup_>   on_setup;
constexpr make_handler_t<on_error_>   on_error;
constexpr make_handler_t<on_success_> on_success;

}}

namespace initializers
{
using ::boost::process::detail::initializers::on_setup  ;
using ::boost::process::detail::initializers::on_error  ;
using ::boost::process::detail::initializers::on_success;


}

}}

#endif
