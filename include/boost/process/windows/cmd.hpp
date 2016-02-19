// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_WINDOWS_CMD_HPP_
#define BOOST_PROCESS_WINDOWS_CMD_HPP_

#include <boost/process/detail/cmd.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace boost
{
namespace process
{
namespace windows
{
namespace detail
{


template <class String>
struct cmd_setter_ : ::boost::process::detail::cmd_setter_
{
    using ::boost::process::detail::arg_setter_::arg_setter_;

    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.cmd_line = cmd_line_.get();
    }
};

}

constexpr boost::process::detail::cmd_ <boost::process::windows::detail::cmd_setter_> cmd;


}
}
}



#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ARGS_HPP_ */
