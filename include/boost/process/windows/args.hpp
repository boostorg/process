// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_WINDOWS_ARGS_HPP_
#define BOOST_PROCESS_WINDOWS_ARGS_HPP_

#include <boost/process/detail/args.hpp>
#include <boost/algorithm/string/trim.hpp>


#if defined( BOOST_WINDOWS_API )
#include <boost/process/windows/args.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/process/windows/args.hpp>

#endif

namespace boost
{
namespace process
{
namespace detail
{
namespace windows
{

typedef std::string native_args;


inline std::string build_args(std::vector<std::string> && data)
{
    std::string st;
    for (auto & arg : data)
    {
        //don't need the argument afterwards so,
        boost::trim(arg);

        if ((arg.front() != '"') && (arg.back() != '"'))
        {
            auto it = std::find(arg.begin(), arg.end(), ' ');//contains space?
            if (it != arg.end())//ok, contains spaces.
            {
                //the first one is put directly onto the output,
                //because then I don't have to copy the whole string
                st += '"';
                arg += '"'; //thats the post one.
            }
        }
        if (!st.empty())//first one does not need a preceeding space
            st += ' ';

        st += arg;
    }
    return st;
}

template< class Executor>
void apply_args(const std::string & data, Executor & e)
{
    e.cmd_line = data.c_str();
}

}



}
}
}



#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ARGS_HPP_ */
