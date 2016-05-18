// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_WINDOWS_GROUP_REF_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_GROUP_REF_HPP_

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/windows/group_handle.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/process/detail/windows/handler.hpp>

namespace boost { namespace process {

namespace detail { namespace windows {



struct group_ref : handler_base_ext
{
    ::boost::detail::winapi::HANDLE_ handle;

    explicit group_ref(::boost::detail::winapi::HANDLE_ h) :
                handle(h)
    {}

    template <class Executor>
    void on_fork_success(Executor& exec) const
    {
        
    }

};

}}}}


#endif /* BOOST_PROCESS_DETAIL_WINDOWS_GROUP_HPP_ */
