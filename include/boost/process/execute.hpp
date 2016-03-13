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
 * \file boost/process/execute.hpp
 *
 * Defines a function to execute a program.
 */

#ifndef BOOST_PROCESS_EXECUTE_HPP
#define BOOST_PROCESS_EXECUTE_HPP

#include <boost/process/config.hpp>
#include <boost/process/executor.hpp>
#include <boost/process/detail/traits.hpp>

#include <boost/hana/partition.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/unique.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/concat.hpp>


#include <utility>

namespace boost { namespace process {



template<typename ...Args>
auto execute(Args&& ... args)
{
    //create a tuple from the argument list
    boost::hana::tuple<Args...> tup(std::forward<Args>(args)...);

    auto par = boost::hana::partition(tup, [](const auto & x){return detail::is_initializer(x);});

    auto inits = boost::hana::first(par);

    auto others = boost::hana::second(par);

    //get the tags of the elements of initializers.
    auto tags = boost::hana::unique(
                    boost::hana::transform(others,
                            [](const auto & x){return detail::initializer_tag(x);}
                        ),
                        [](const auto & a, const auto& b)
                        {
                            return boost::hana::decltype_(a) == boost::hana::decltype_(b);
                        });

    //combine the other argument to get initializers.
    auto other_inits = boost::hana::transform(tags,
            [&others](auto tag)
            {
                 //get the initializers fitting the tag
                auto tup = boost::hana::filter(others,
                        [&](auto && value)
                        {
                            return boost::hana::decltype_(detail::initializer_tag(value))
                                == boost::hana::decltype_(tag);
                        });
                return detail::make_initializer(tag, tup);
            });

    //exec only takes a refernenc eto initializers.
    auto initializers = boost::hana::concat(inits, std::move(other_inits));
    auto exec = boost::process::detail::api::make_executor(initializers);
    return exec();
}




}}

#endif
