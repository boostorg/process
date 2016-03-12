// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_
#define BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_

#include <boost/process/detail/exe.hpp>
#include <boost/process/detail/args.hpp>

#include <iterator>

namespace boost { namespace process { namespace detail {

struct exe_builder
{
    std::string exe;
    std::vector<std::string> args;

    const char* get_exe     () const { return exe.c_str();}
    const char* get_cmd_line() const { return nullptr;} //TODO: Implement.

    void operator()(std::string & data)
    {
        if (exe.empty())
            exe = std::move(data);
    }
    void operator()(std::vector<std::string> & data)
    {
        if (data.empty())
            return;

        auto itr = std::make_move_iterator(data.begin());
        auto end = std::make_move_iterator(data.end());

        if (exe.empty())
        {
            exe = *itr;
            itr++;
        }
        args.insert(args.end(), itr, end);
    }
    void operator()(exe_setter_<std::string> & data)
    {
        exe = std::move(data.exe_);
    }
    template<typename Range>
    void operator()(arg_setter_<Range, false> & data)
    {
        args.assign(
                std::make_move_iterator(data._args.begin()),
                std::make_move_iterator(data._args.end()));
    }
    template<typename Range>
    void operator()(arg_setter_<Range, true> & data)
    {
        args.insert(args.end(),
                std::make_move_iterator(data._args.begin()),
                std::make_move_iterator(data._args.end()));
    }

};

}}}



#endif /* INCLUDE_BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_ */
