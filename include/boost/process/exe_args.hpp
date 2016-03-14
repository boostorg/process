// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_
#define BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_

#include <boost/process/config.hpp>
#include <boost/process/detail/exe.hpp>
#include <boost/process/detail/args.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/cmd_or_exe.hpp>

#include <boost/hana/tuple.hpp>
#include <boost/hana/for_each.hpp>

#include <iterator>



namespace boost { namespace process { namespace detail {


struct exe_args_init : boost::process::detail::handler_base
{
    exe_args_init(const std::string & exe)
                : exe(exe), args({}) {};
    exe_args_init(std::string && exe)
                : exe(std::move(exe)), args({}) {};

    exe_args_init(std::string && exe, std::vector<std::string> && args)
            : exe(std::move(exe)), args(api::build_args(std::move(args))) {};
    template <class Executor>
    void on_setup(Executor& exec) const
    {
        detail::api::apply_exe (exe,  exec);
        detail::api::apply_args(args, exec);
    }
    typedef api::native_args native_args;
private:
    std::string exe;
    native_args args;
};

struct exe_builder
{
    std::string exe;
    std::vector<std::string> args;

    const char* get_exe     () const { return exe.c_str();}
    const char* get_cmd_line() const { return nullptr;} //TODO: Implement.

    void operator()(boost::filesystem::path && data)
        {
            if (exe.empty())
                exe = data.string();
            else
                args.push_back(data.string());
        }

    void operator()(std::string && data)
    {
        if (exe.empty())
            exe = std::move(data);
        else
            args.push_back(std::move(data));
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

    exe_args_init get()
    {
        return exe_args_init(std::move(exe), std::move(args));
    }
};

//everything with at least to parameters will use the exe_builder.
template<typename T, typename... Rest>
inline exe_args_init make_initializer(cmd_or_exe_tag&, boost::hana::tuple<T*,  Rest*...> & lst)
{
    exe_builder b;
    boost::hana::for_each(lst, [&](auto & value){b(std::move(*value));});

    return b.get();
}

inline exe_args_init make_initializer(cmd_or_exe_tag&, boost::hana::tuple<const boost::filesystem::path*> & cmd)
{
    const auto & value = *boost::hana::at(cmd, boost::hana::size_c<0>);
    return exe_args_init(value.string());
}

inline exe_args_init make_initializer(cmd_or_exe_tag&, boost::hana::tuple<boost::filesystem::path*> & cmd)
{
    const auto & value = *boost::hana::at(cmd, boost::hana::size_c<0>);
    return exe_args_init(value.string());
}

}}}



#endif /* INCLUDE_BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_ */
