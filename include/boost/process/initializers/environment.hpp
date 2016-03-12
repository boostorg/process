// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_INITIALIZERS_ENVIRONMENT_HPP_
#define BOOST_PROCESS_INITIALIZERS_ENVIRONMENT_HPP_

#include <boost/process/detail/environment.hpp>
#include <boost/blank.hpp>

namespace boost { namespace process { namespace initializers {

inline std::string join_var_values(const std::vector<std::string> & vec)
{
    std::string value;
    for (auto & v : vec)
    {
        if (&v != &vec.front())
            value += ';';
        value += v;
    }
    return value;
}


struct env_set
{
    environment env_;
};

struct env_reset {};

struct env_var_append
{
    std::string id;
    std::string value;
};

struct env_var_set
{
    std::string id;
    std::string value;
};

struct env_var_reset
{
    std::string id;
};

struct env_var_proxy
{
    std::string id;
    env_var_set operator=(const std::string & val) {return env_var_set(std::move(id), val);}
    env_var_set operator=(std::string && val) {return env_var_set(std::move(id), std::move(val));}
    env_var_set operator=(const std::vector<std::string>& vec) {return this->operator=(join_var_values(vec));}

    env_var_append operator+=(const std::string & val) {return env_var_append(std::move(id), val);}
    env_var_append operator+=(std::string && val)      {return env_var_append(std::move(id), std::move(val));}
    env_var_append operator+=(const std::vector<std::string>& vec) {return this->operator=(join_var_values(vec));}

    env_var_reset operator=(std::nullptr_t) { return env_var_reset();};
    env_var_reset operator=(boost::blank)   { return env_var_reset();};
};

struct env_
{
    env_var_proxy operator[](const char* id) const
    {
        return env_var_proxy{id};
    }
    env_var_set operator+=(const std::string & id, const std::string & value) const
    {
        return env_var_set(id, value);
    }
    env_var_set operator+=(const std::string & id, const std::vector<std::string> & vec) const
    {

        return env_var_set(id, join_var_values(vec));
    }
    env_var_reset operator-=(const std::string & id) const
    {
        return env_var_reset(id);
    }
    env_set operator=(const environment &  env) const {return env_set{env};}
    env_set operator=(      environment && env) const {return env_set{std::move(env)};}

    env_reset operator=(boost::blank)   {return env_reset();}
    env_reset operator=(std::nullptr_t) {return env_reset();}

};

std::false_type is_env(const auto &) {return std::false_type();}
std::true_type  is_env(const env_set &) {return std::true_type();}
std::true_type  is_env(const env_reset &) {return std::true_type();}
std::true_type  is_env(const env_var_append &) {return std::true_type();}
std::true_type  is_env(const env_var_set &) {return std::true_type();}
std::true_type  is_env(const env_var_reset &) {return std::true_type();}

constexpr static env_ env;

}

using boost::process::initializers::env;

}}

#endif /* INCLUDE_BOOST_PROCESS_INITIALIZERS_ENVIRONMENT_HPP_ */
