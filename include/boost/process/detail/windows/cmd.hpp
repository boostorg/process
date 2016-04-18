// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_WINDOWS_CMD_HPP_
#define BOOST_PROCESS_WINDOWS_CMD_HPP_

#include <string>

namespace boost
{
namespace process
{
namespace detail
{
namespace windows
{



struct cmd_setter_ : ::boost::process::detail::handler_base
{
    cmd_setter_(std::string && cmd_line)      : _cmd_line(std::move(cmd_line)) {}
    cmd_setter_(const std::string & cmd_line) : _cmd_line(cmd_line) {}
    template <class Executor>
    void on_setup(Executor& exec) const
    {
   		exec.cmd_line = _cmd_line.c_str();
    }
public:
    std::string _cmd_line;
};

}


}
}
}



#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ARGS_HPP_ */
