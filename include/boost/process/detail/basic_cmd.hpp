// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_BASIC_CMD_HPP_
#define BOOST_PROCESS_DETAIL_BASIC_CMD_HPP_

#include <boost/process/detail/config.hpp>

#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/traits/cmd_or_exe.hpp>

#if defined( BOOST_WINDOWS_API )
#include <boost/process/detail/windows/args.hpp>
#include <boost/process/detail/windows/exe.hpp>
#include <boost/process/detail/windows/cmd.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/process/detail/posix/args.hpp>
#include <boost/process/detail/posix/exe.hpp>
#include <boost/process/detail/posix/cmd.hpp>
#endif

#include <iterator>



namespace boost { namespace process { namespace detail {

struct exe_setter_
{
    std::string exe_;
    exe_setter_(std::string && str)      : exe_(std::move(str)) {}
    exe_setter_(const std::string & str) : exe_(str) {}

};


template <class String, bool Append = false>
struct arg_setter_ : ::boost::process::detail::handler_base
{
    std::vector<String> _args;

    typedef typename std::vector<String>::iterator       iterator;
    typedef typename std::vector<String>::const_iterator const_iterator;

    template<typename Range>
    arg_setter_(Range && str) :
            _args(std::make_move_iterator(std::begin(str)),
                  std::make_move_iterator(std::end(str))) {}

    iterator begin() {return _args.begin();}
    iterator end()   {return _args.end();}
    const_iterator begin() const {return _args.begin();}
    const_iterator end()   const {return _args.end();}
    arg_setter_(std::string && s) : _args({std::move(s)}) {}
    arg_setter_(const std::string & s) : _args({s}) {}
    arg_setter_(const char* s) : _args({std::move(s)}) {}

    template<std::size_t Size>
    arg_setter_(const char (&s) [Size]) : _args({s}) {}
};


struct exe_cmd_init : boost::process::detail::handler_base
{
    exe_cmd_init(const std::string & exe, bool cmd_only = false)
                : exe(exe), args({}), cmd_only(cmd_only) {};
    exe_cmd_init(std::string && exe, bool cmd_only = false)
                : exe(std::move(exe)), args({}), cmd_only(cmd_only) {};

    exe_cmd_init(std::string && exe, std::vector<std::string> && args)
            : exe(std::move(exe)), args(api::build_args(std::move(args))), cmd_only(false) {};
    template <class Executor>
    void on_setup(Executor& exec) const
    {
        if (cmd_only)
            detail::api::apply_cmd (exe, exec);
        else
        {
            detail::api::apply_exe (exe,  exec);
            detail::api::apply_args(args, exec);
        }
    }
    typedef api::native_args native_args;
private:
    std::string exe;
    native_args args;
    bool cmd_only;
};

struct exe_builder
{
    //set by path, because that will not be interpreted as a cmd
    bool not_cmd = false;
    std::string exe;
    std::vector<std::string> args;

    const char* get_exe     () const { return exe.c_str();}
    const char* get_cmd_line() const { return nullptr;} //TODO: Implement.

    void operator()(const boost::filesystem::path & data)
    {
        not_cmd = true;
        if (exe.empty())
            exe = data.string();
        else
            args.push_back(data.string());
    }

    void operator()(const std::string & data)
    {
        if (exe.empty())
            exe = std::move(data);
        else
            args.push_back(std::move(data));
    }
    void operator()(const char* data)
    {
        if (exe.empty())
            exe = data;
        else
            args.push_back(data);
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
    void operator()(exe_setter_ && data)
    {
        not_cmd = true;
        exe = std::move(data.exe_);
    }
    template<typename Range>
    void operator()(arg_setter_<Range, false> && data)
    {
        args.assign(
                std::make_move_iterator(data._args.begin()),
                std::make_move_iterator(data._args.end()));
    }
    template<typename Range>
    void operator()(arg_setter_<Range, true> && data)
    {
        args.insert(args.end(),
                std::make_move_iterator(data._args.begin()),
                std::make_move_iterator(data._args.end()));
    }
//    template<typename Range>
//    void operator()(const arg_setter_<Range, false> & data)
//    {
//        args.assign(data._args.begin(), data._args.end());
//    }
//    template<typename Range>
//    void operator()(const arg_setter_<Range, true> & data)
//    {
//        args.insert(args.end(), data._args.begin(), data._args.end());
//    }

    exe_cmd_init get_initializer()
    {
        if (not_cmd || !args.empty())
            return exe_cmd_init(std::move(exe), std::move(args));
        else
            return exe_cmd_init(std::move(exe), true);
    }
    typedef exe_cmd_init result_type;
};

template<>
struct initializer_builder<cmd_or_exe_tag>
{
    typedef exe_builder type;
};



}}}



#endif /* INCLUDE_BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_ */
