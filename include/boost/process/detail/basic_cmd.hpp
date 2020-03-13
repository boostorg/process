// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_BASIC_CMD_HPP_
#define BOOST_PROCESS_DETAIL_BASIC_CMD_HPP_

#include <boost/process/detail/config.hpp>

#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/traits/cmd_or_exe.hpp>
#include <boost/process/detail/traits/wchar_t.hpp>

#if defined( BOOST_WINDOWS_API )
#include <boost/process/detail/windows/basic_cmd.hpp>
#include <boost/process/detail/windows/cmd.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/process/detail/posix/basic_cmd.hpp>
#include <boost/process/detail/posix/cmd.hpp>
#endif

#include <boost/process/shell.hpp>

#include <iterator>


namespace boost { namespace process { namespace detail {

template<typename Char>
struct exe_setter_
{
    using value_type = Char;
    using string_type = std::basic_string<Char>;

    string_type exe_;

    exe_setter_(string_type str) : exe_(std::move(str)) {}
};

template<> struct is_wchar_t<exe_setter_<wchar_t>> : std::true_type {};

template<>
struct char_converter<char, exe_setter_<wchar_t>>
{
    static exe_setter_<char> conv(const exe_setter_<wchar_t> & in)
    {
        return {::boost::process::detail::convert(in.exe_)};
    }
};

template<>
struct char_converter<wchar_t, exe_setter_<char>>
{
    static exe_setter_<wchar_t> conv(const exe_setter_<char> & in)
    {
        return {::boost::process::detail::convert(in.exe_)};
    }
};

template <typename Char, bool Append >
struct arg_setter_
{
    using value_type = Char;
    using string_type = std::basic_string<value_type>;
    std::vector<string_type> _args;

    using iterator       = typename std::vector<string_type>::iterator;
    using const_iterator = typename std::vector<string_type>::const_iterator;

    arg_setter_(string_type str) : _args({std::move(str)}) {}

    // Disambiguates between character pointers and the single-argument template constructor.
    arg_setter_(const value_type* s)   : _args({std::move(s)}) {}

    // Disambiguates between character arrays and the single-argument template constructor.
    template<std::size_t Size>
    arg_setter_(const value_type (&s) [Size]) : _args({s}) {}

    template<typename Iterator>
    arg_setter_(Iterator && begin, Iterator && end) :
            _args(std::forward<Iterator>(begin), std::forward<Iterator>(end)) {}

    // TODO: Doesn't distinguish whether the provided parameter can be moved from, such as with std::make_move_iterator.
    template<typename Range>
    arg_setter_(Range && str) :
            _args(std::begin(str),
                  std::end(str)) {}

    iterator begin() {return _args.begin();}
    iterator end()   {return _args.end();}
    const_iterator begin() const {return _args.begin();}
    const_iterator end()   const {return _args.end();}
};

template<> struct is_wchar_t<arg_setter_<wchar_t, true >> : std::true_type {};
template<> struct is_wchar_t<arg_setter_<wchar_t, false>> : std::true_type {};

template<>
struct char_converter<char, arg_setter_<wchar_t, true>>
{
    static arg_setter_<char, true> conv(const arg_setter_<wchar_t, true> & in)
    {
        std::vector<std::string> vec(in._args.size());
        std::transform(in._args.begin(), in._args.end(), vec.begin(),
                [](const std::wstring & ws)
                {
                    return ::boost::process::detail::convert(ws);
                });
        return {vec};
    }
};

template<>
struct char_converter<wchar_t, arg_setter_<char, true>>
{
    static arg_setter_<wchar_t, true> conv(const arg_setter_<char, true> & in)
    {
        std::vector<std::wstring> vec(in._args.size());
        std::transform(in._args.begin(), in._args.end(), vec.begin(),
                [](const std::string & ws)
                {
                    return ::boost::process::detail::convert(ws);
                });

        return {vec};
    }
};

template<>
struct char_converter<char, arg_setter_<wchar_t, false>>
{
    static arg_setter_<char, false> conv(const arg_setter_<wchar_t, false> & in)
    {
        std::vector<std::string> vec(in._args.size());
        std::transform(in._args.begin(), in._args.end(), vec.begin(),
                [](const std::wstring & ws)
                {
                    return ::boost::process::detail::convert(ws);
                });
        return {vec};    }
};

template<>
struct char_converter<wchar_t, arg_setter_<char, false>>
{
    static arg_setter_<wchar_t, false> conv(const arg_setter_<char, false> & in)
    {
        std::vector<std::wstring> vec(in._args.size());
        std::transform(in._args.begin(), in._args.end(), vec.begin(),
                [](const std::string & ws)
                {
                    return ::boost::process::detail::convert(ws);
                });
        return {vec};
    }
};

using api::exe_cmd_init;

template<typename Char>
struct exe_builder
{
    //set by path, because that will not be interpreted as a cmd
    bool not_cmd = false;
    bool shell   = false;
    using string_type = std::basic_string<Char>;
    string_type exe;
    std::vector<string_type> args;

    void operator()(string_type data)
    {
        if (exe.empty())
            exe = std::move(data);
        else
            args.push_back(std::move(data));
    }

    void operator()(const Char *data)
    {
        exe_builder::operator()(string_type{data});
    }

    void operator()(const boost::filesystem::path & data)
    {
        not_cmd = true;
        exe_builder::operator()(data.native());
    }

    void operator()(shell_) {shell = true;}

    void operator()(std::vector<string_type> data)
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

    void operator()(exe_setter_<Char> data)
    {
        not_cmd = true;
        exe = std::move(data.exe_);
    }

    void operator()(arg_setter_<Char, false> data)
    {
        args = std::move(data._args);
    }

    void operator()(arg_setter_<Char, true> data)
    {
        args.insert(args.end(),
                std::make_move_iterator(data._args.begin()),
                std::make_move_iterator(data._args.end()));
    }

    api::exe_cmd_init<Char> get_initializer()
    {
        if (not_cmd || !args.empty())
        {
            if (shell)
                return api::exe_cmd_init<Char>::exe_args_shell(std::move(exe), std::move(args));
            else
                return api::exe_cmd_init<Char>::exe_args(std::move(exe), std::move(args));
        }
        else
            if (shell)
                return api::exe_cmd_init<Char>::cmd_shell(std::move(exe));
            else
                return api::exe_cmd_init<Char>::cmd(std::move(exe));

    }
    using result_type = api::exe_cmd_init<Char>;
};

template<>
struct initializer_builder<cmd_or_exe_tag<char>>
{
    using type = exe_builder<char>;
};

template<>
struct initializer_builder<cmd_or_exe_tag<wchar_t>>
{
    using type = exe_builder<wchar_t>;
};

}}}

#endif /* BOOST_PROCESS_DETAIL_EXE_BUILDER_HPP_ */
