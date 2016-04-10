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
template<class StringType, class Executor>
void apply_cmd(const StringType & cmd_line, Executor & e)
{
    e.cmd_line = cmd_line;
}

template<class StringType, class Executor>
void apply_cmd(const std::basic_string<StringType> & cmd_line, Executor & e)
{
    e.cmd_line = cmd_line.c_str();
}

}


}
}
}



#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ARGS_HPP_ */
