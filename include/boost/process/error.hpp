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
#include <system_error>
#include <boost/hana/tuple.hpp>
#include <boost/hana/find_if.hpp>
#include <boost/hana/size.hpp>

#include <boost/type_index.hpp>

namespace boost { namespace process {

namespace detail {

struct throw_on_error_ : ::boost::process::detail::handler_base
{
    constexpr throw_on_error_() {};

    template <class Executor>
    void on_error(Executor&, const std::error_code & ec) const
    {
        throw std::system_error(ec, "process creation failed");
    }

    const throw_on_error_ &operator()() const {return *this;}
};

struct ignore_error_ : ::boost::process::detail::handler_base
{
    constexpr ignore_error_() {};
};

struct set_on_error : ::boost::process::detail::handler_base
{
    set_on_error(const set_on_error&) = default;
    explicit set_on_error(std::error_code &ec) : ec_(ec) {}

    template <class Executor>
    void on_error(Executor&, const std::error_code & ec) const
    {
        ec_ = ec;
    }

private:
    std::error_code &ec_;
};

struct error_
{
    constexpr error_() {}
    set_on_error operator()(std::error_code &ec) const {return set_on_error(ec);}
    set_on_error operator= (std::error_code &ec) const {return set_on_error(ec);}

};

struct error_tag {};

template<typename T>
std::false_type is_error_handler(const T & ) {return {};}

inline std::true_type is_error_handler(const set_on_error &)    {return {};}
inline std::true_type is_error_handler(const throw_on_error_ &) {return {};}
inline std::true_type is_error_handler(const ignore_error_ &)   {return {};}
inline std::true_type is_initializer  (const set_on_error &)    {return {};}
inline std::true_type is_initializer  (const throw_on_error_ &) {return {};}
inline std::true_type is_initializer  (const ignore_error_ &)   {return {};}


//note: is a tuple of pointers to initializers
template<typename Tuple>
constexpr auto has_error_handler(Tuple &t)
{
    auto err_handlers = boost::hana::find_if(t, [](auto * p){ return is_error_handler(*p);});

    return boost::hana::not_(boost::hana::size(err_handlers) == hana::size_c<0>);
}


inline error_tag initializer_tag(const std::error_code&) {return {};}


inline set_on_error make_initializer(error_tag, boost::hana::tuple<std::error_code*> & err)
{
    std::error_code * value = boost::hana::at(err, boost::hana::size_c<0>);
    set_on_error setter(*value);
    return setter;
}


}

constexpr static boost::process::detail::error_ error;
constexpr static boost::process::detail::error_ error_ref;
constexpr static boost::process::detail::error_ error_code;
constexpr static boost::process::detail::throw_on_error_ throw_on_error;


}}

#endif
