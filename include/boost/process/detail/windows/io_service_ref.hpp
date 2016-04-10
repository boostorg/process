// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_IO_SERVICE_REF_HPP_
#define BOOST_PROCESS_WINDOWS_IO_SERVICE_REF_HPP_

#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/windows/async_handler.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/windows/object_handle.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/transform.hpp>
#include <boost/type_index.hpp>
#include <functional>
#include <memory>

namespace boost { namespace process { namespace detail { namespace windows {



//Also set's up waiting for the exit, so it can close async stuff.
struct io_service_ref : boost::process::detail::handler_base
{
    io_service_ref(boost::asio::io_service & ios) : ios(ios)
    {

    }
    boost::asio::io_service &get() {return ios;};
    template <class Executor>
    void on_success(Executor& exec) const
    {
          ::boost::detail::winapi::PROCESS_INFORMATION_ & proc = exec.proc_info;
          auto process_handle = proc.hProcess;

          //must be on the heap so I can move it into the lambda.
          auto handle = std::make_unique<boost::asio::windows::object_handle>(ios, proc.hProcess);


          auto asyncs = boost::hana::filter   (exec.seq, [ ](auto *p){return is_async_handler(*p);});
          auto funcs  = boost::hana::transform(asyncs,   [&](auto *p){return p->on_exit_handler(exec);});

          auto handle_p = handle.get();
          handle_p->async_wait(
                  [funcs, _ = std::move(handle)](const boost::system::error_code & ec_in)
                  {
                      auto ec = std::error_code(ec_in.value(), std::system_category());
                      boost::hana::for_each(funcs, [&](const auto & func){func(ec);});
                  });



    }

private:
    boost::asio::io_service &ios;
};

}}}}

#endif /* BOOST_PROCESS_WINDOWS_IO_SERVICE_REF_HPP_ */
