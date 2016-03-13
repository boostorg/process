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

#include <boost/process/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <system_error>
#include <boost/hana/tuple.hpp>

namespace boost { namespace process {

namespace detail {

struct set_on_error : ::boost::process::detail::handler_base
{
    set_on_error(const set_on_error&) = default;
    explicit set_on_error(std::error_code &ec) : ec_(ec) {}

    template <class Executor>
    void on_error(Executor&, const std::error_code & ec) const
    {
        std::cout << "EC2: " << &ec << std::endl;
        std::cout << "EC3: " << &ec_ << std::endl;

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

inline std::true_type is_error_handler(const set_on_error &) {return {};}
inline std::true_type is_initializer  (const set_on_error &) {return {};}

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


}}

#endif
