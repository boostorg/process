// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_WINDOWS_EXE_HPP_
#define BOOST_PROCESS_WINDOWS_EXE_HPP_

#include <boost/process/detail/exe.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace boost
{
namespace process
{
namespace detail
{
namespace windows
{

template<class StringType, class Executor>
void apply_exe(const StringType & exe, Executor & e)
{
    e.exe = exe.c_str();
}

}



}
}
}



#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ARGS_HPP_ */
