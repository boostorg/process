// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_POSIX_CMD_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_CMD_HPP_

#include <string>
#include <vector>

namespace boost
{
namespace process
{
namespace detail
{
namespace posix
{



inline std::vector<std::string> build_cmd(const std::string & value) 
{
   	std::vector<std::string>  ret;
    

   	bool in_quotes = false;
   	auto beg = value.begin();
    for (auto itr = value.begin(); itr != value.end(); itr++)
    {
        if (*itr == '"')
        	in_quotes = !in_quotes;

        if (!in_quotes && (*itr == ' '))
        {
        	if (itr != beg)
        	{
        		ret.emplace_back(beg, itr);
        		beg = itr + 1;
        	}
        }
    }
    if (beg != value.end())
    	ret.emplace_back(beg, value.end());

    return ret;
}

struct cmd_setter_ : ::boost::process::detail::handler_base
{
    cmd_setter_(std::string && cmd_line)      : _cmd_line(api::build_cmd(std::move(cmd_line))) {}
    cmd_setter_(const std::string & cmd_line) : _cmd_line(api::build_cmd(cmd_line)) {}
    template <class Executor>
    void on_setup(Executor& exec) 
    {
        exec.cmd_line = &_cmd_impl.front();
    }
private:
    static std::vector<const char*> make_cmd(const std::vector<std::string> & args);
    std::vector<std::string> _cmd_line;
    std::vector<const char*> _cmd_impl  = make_cmd(_cmd_line);
};


std::vector<const char*> cmd_setter_::make_cmd(const std::vector<std::string> & args)
{
    std::vector<const char*> vec;

    for (auto & v : args)
        vec.push_back(v.c_str());

    vec.push_back(nullptr);

    return vec;
}

}}}}

#endif