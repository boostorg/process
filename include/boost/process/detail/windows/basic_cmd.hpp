// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_WINDOWS_BASIC_CMD_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_BASIC_CMD_HPP_

#include <boost/algorithm/string/trim.hpp>
#include <boost/process/detail/windows/handler.hpp>

#include <vector>
#include <string>


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
                arg.insert(arg.begin(), '"');
                arg += '"'; //thats the post one.
            }
        }
        if (!st.empty())//first one does not need a preceeding space
            st += ' ';

        st += arg;
    }
    return st;
}


struct exe_cmd_init : handler_base_ext
{
    exe_cmd_init(const std::string & exe, bool cmd_only = false)
                : exe(exe), args({}), cmd_only(cmd_only) {};
    exe_cmd_init(std::string && exe, bool cmd_only = false)
                : exe(std::move(exe)), args({}), cmd_only(cmd_only) {};

    exe_cmd_init(std::string && exe, std::vector<std::string> && args)
            : exe(std::move(exe)), args(build_args(std::move(args))), cmd_only(false) {};
    template <class Executor>
    void on_setup(Executor& exec) const
    {

        if (cmd_only && args.empty())
            exec.cmd_line = exe.c_str();
        else
        {
            exec.exe = exe.c_str();
            exec.cmd_line = args.c_str();
        }
    }
    static exe_cmd_init exe_args(std::string&& exe, std::vector<std::string> && args)
    {
        return exe_cmd_init(std::move(exe), std::move(args));
    }
    static exe_cmd_init cmd(std::string&& cmd)
    {
        return exe_cmd_init(std::move(cmd), true);
    }
private:
    std::string exe;
    std::string args;
    bool cmd_only;
};

}



}
}
}



#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ARGS_HPP_ */
