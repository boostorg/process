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
#include <boost/process/detail/string_traits.hpp>
#include <boost/process/detail/args.hpp>
#include <boost/process/detail/cmd.hpp>
#include <boost/process/detail/exe.hpp>
#include <boost/process/detail/exe_builder.hpp>
#include <boost/process/detail/env.hpp>

#include <boost/hana/traits.hpp>
#include <boost/hana/or.hpp>
#include <boost/hana/not.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/count_if.hpp>
#include <boost/hana/unique.hpp>
#include <boost/hana/tuple.hpp>

namespace boost { namespace process {

namespace detail {

template<typename ...Args>
int make_execute(Args...) {return 42;}

int launch_process(int) {return -2;}




template<typename Tuple>
auto execute_impl(std::true_type, Tuple && tpl)
{
    namespace hana = boost::hana;

    constexpr auto trait = hana::or_(hana::trait<is_string_t>,
            hana::trait<is_string_list_t>,
            hana::trait<is_exe_setter_t>,
            hana::trait<is_arg_setter_t>);

    constexpr auto trait_env = hana::trait<is_env_t>;

    auto filtered = hana::filter(tpl, trait);

    auto env_tup = hana::filter(tpl, trait_env);

    exe_builder exe;
    hana::for_each(filtered, exe);

    auto env = build_env(std::move(env_tup));

    auto the_rest = hana::filter(tpl, hana::not_(trait));

    //TODO: Add an assertion for

    //the executor
    auto exec = make_executor(std::move(the_rest));

    return launch_process(exe.get(), std::move(env), exec);
}


template<typename Tuple>
auto execute_impl(std::false_type, Tuple && tpl)
{
    namespace hana = boost::hana;

    constexpr auto trait = hana::or_(hana::trait<is_string_t>,
            hana::trait<is_cmd_setter_t>);

    constexpr auto trait_env = hana::trait<is_env_t>;

    auto filtered = hana::filter(tpl, trait);

    auto env_tup = hana::filter(tpl, trait_env);


    cmd_builder cmd;
    hana::for_each(filtered, cmd);

    env_builder env;
    hana::for_each(env_tup, env);

    auto the_rest = hana::filter(tpl, hana::not_(trait));

    auto exec = make_executor(std::move(exe), std::move(env), std::move(the_rest));

    return launch_process(exec);

}

}

template<typename ...Args>
auto execute(Args&& ... args)
{
    namespace hana = boost::hana;
    //create a tuple from the argument list
    auto tup = hana::make_tuple(std::forward<Args>(args)...);

    //the count of strings
    constexpr auto str_cnt  = hana::count_if(tup, boost::hana::trait<detail::is_string_t>);

    //the count of string lists, currently for vectors
    constexpr auto str_list_cnt = hana::count_if(tup, boost::hana::trait<detail::is_string_list_t>);

    //only execution by cmd, i.e. one string
    constexpr bool str_only = (str_cnt == 1) && (str_list_cnt == 0);

    //contains arg or exe
    constexpr bool arg_found = hana::count_if(tup, hana::trait<detail::is_arg_setter_t>);

    constexpr bool exe_found = hana::count_if(tup, hana::trait<detail::is_exe_setter_t>);
    constexpr bool arg_or_exe = arg_found || exe_found;


    constexpr bool cmd_found = hana::count_if(tup, hana::trait<detail::is_cmd_setter_t>);
    constexpr bool cmd_only = ( str_only ^ cmd_found ) & ~arg_or_exe;

    //cmd type
    using cmd_only_t = std::integral_constant<bool, cmd_only>;

    static_assert(cmd_only ^ arg_or_exe, "Exe/Arg and cmd syntax cannot be mixed");
    static_assert(arg_or_exe || cmd_found, "No command provided");

    //call either with exe/args or cmd
    return detail::execute_impl(cmd_only_t(), std::move(tup));
}





}}

#endif
