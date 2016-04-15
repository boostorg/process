// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_WINDOWS_ATTACHED_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_ATTACHED_HPP_

#include <boost/process/detail/config.hpp>
#include <boost/detail/winapi/jobs.hpp>

#include <boost/process/detail/windows/handler.hpp>

namespace boost { namespace process { namespace detail { namespace windows {


struct attached : public ::boost::process::detail::handler_base
{
public:
    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.job_object = ::boost::detail::winapi::create_job_object(e.proc_attrs, nullptr);
    }
};


}}}}



#endif /* BOOST_PROCESS_DETAIL_HANDLER_HPP_ */
