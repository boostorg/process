// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_ENV_HPP_
#define BOOST_PROCESS_DETAIL_ENV_HPP_

#include <boost/process/environment.hpp>
#include <boost/none.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/for_each.hpp>

namespace boost { namespace process { namespace detail {

template<typename T>
struct env_set
{
    std::string key;
    T value;
};

template<typename T>
struct env_append
{
    std::string key;
    T value;
};

struct env_reset
{
    std::string key;
};

struct env_init
{
    environment env;
};

struct env_proxy
{
    std::string key;

    env_set<std::string> operator=(const std::string & value)
    {
        return {std::move(key), value};
    }
    env_set<std::vector<std::string>> operator=(const std::vector<std::string> & value)
    {
        return {std::move(key), value};
    }
    env_set<std::initializer_list<char*>> operator=(const std::initializer_list<char*> & value)
    {
        return {std::move(key), value};
    }

    env_append<std::string> operator+=(const std::string & value)
    {
        return {std::move(key), value};
    }
    env_append<std::vector<std::string>> operator+=(const std::vector<std::string> & value)
    {
        return {std::move(key), value};
    }
    env_append<std::initializer_list<char*>> operator+=(const std::initializer_list<char*> & value)
    {
        return {std::move(key), value};
    }

    env_reset operator=(boost::none_t)
    {
        return {std::move(key)};
    }
};

struct env_
{
    constexpr env_() {};

    env_set<std::string> operator()(const std::string & key, const std::string & value) const
    {
        return {key, value};
    }
    env_set<std::vector<std::string>> operator()(const std::string & key, const std::vector<std::string> & value) const
    {
        return {key, value};
    }
    env_set<std::vector<char*>> operator()(const std::string & key, const std::initializer_list<char*> & value) const
    {
        return {key, std::vector<char*>(value)};
    }
    env_reset operator()(const std::string & key, boost::none_t)
    {
        return {key};
    }
    env_proxy operator[](const std::string & key)
    {
        return {key};
    }
    env_init operator()(const environment & env) const
    {
        return {env};
    }
    env_init operator= (const environment & env) const
    {
        return {env};
    }
};


struct env_builder
{
    environment env;

    void operator()(env_init & ei)
    {
        ei.env = std::move(ei.env);
    }
    template<typename T>
    void operator()(env_set<T> & es)
    {
        //env.set(es.key, es.value_to_string());
    }
    void operator()(env_reset & es)
    {
        env.erase(es.key);
    }
    template<typename T>
    void operator()(env_append<T> & es)
    {
        //env.set(es.key, es.value_to_string());
    }
};


template<typename ...Args>
auto build_env(boost::hana::tuple<Args...> && env_tup)
{
    env_builder env;
    hana::for_each(env_tup, env);
    return env;
}

auto build_env(boost::hana::tuple<> && e)
{
    return boost::process::empty_env();
}

template<class String>
inline constexpr std:: true_type is_env_setter(const env_set<String>&) {return {};}
template<class String>
inline constexpr std:: true_type is_env_setter(const env_append<String>&) {return {};}
inline constexpr std:: true_type is_env_setter(const env_reset&) {return {};}

inline constexpr std:: true_type is_env_setter(const  environment&) {return {};}
inline constexpr std:: true_type is_env_setter(const wenvironment&) {return {};}

inline constexpr std:: true_type is_env_setter(const  empty_env&) {return {};}
inline constexpr std:: true_type is_env_setter(const wempty_env&) {return {};}

template<typename T>
inline constexpr std::false_type is_env_setter(const T &) {return {};}

template<typename T>
struct is_env_t : decltype(is_env_setter(T())) {};


constexpr static env_ env;
}

using boost::process::detail::env;

}}

#endif /* INCLUDE_BOOST_PROCESS_DETAIL_ENV_HPP_ */
