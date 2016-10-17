// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/** \file boost/process/async.hpp

  The header which provides the basic asynchrounous features.
  It provides the on_exit property, which allows callbacks when the process exits.
  It also implements the necessary traits for passing an boost::asio::io_service,
  which is needed for asynchronous communication.

  It also pulls the boost::asio::buffer into the boost::process namespace for convenience.

\xmlonly
<programlisting>
namespace boost {
  namespace process {
    <emphasis>unspecified</emphasis> buffer;
    <emphasis>unspecified</emphasis> <globalname alt="boost::process::on_exit">on_exit</globalname>;
  }
}
</programlisting>

\endxmlonly
  */

#ifndef BOOST_PROCESS_DETAIL_ASYNC_HPP_
#define BOOST_PROCESS_DETAIL_ASYNC_HPP_

#include <boost/process/detail/traits.hpp>
#include <boost/process/detail/on_exit.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffer.hpp>
#include <type_traits>
#include <boost/fusion/iterator/deref.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/io_service_ref.hpp>
#include <boost/process/detail/posix/async_in.hpp>
#include <boost/process/detail/posix/async_out.hpp>
#include <boost/process/detail/posix/on_exit.hpp>

#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/io_service_ref.hpp>
#include <boost/process/detail/windows/async_in.hpp>
#include <boost/process/detail/windows/async_out.hpp>
#include <boost/process/detail/windows/on_exit.hpp>
#endif

namespace boost { namespace process { namespace detail {

struct async_tag;

template<typename T>
struct is_io_service : std::false_type {};
template<>
struct is_io_service<api::io_service_ref> : std::true_type {};

template<typename Tuple>
inline asio::io_service& get_io_service(const Tuple & tup)
{
    auto& ref = *boost::fusion::find_if<is_io_service<boost::mpl::_>>(tup);
    return ref.get();
}

struct async_builder
{
    boost::asio::io_service * ios;

    void operator()(boost::asio::io_service & ios) {this->ios = &ios;};

    typedef api::io_service_ref result_type;
    api::io_service_ref get_initializer() {return api::io_service_ref (*ios);};
};


template<>
struct initializer_builder<async_tag>
{
    typedef async_builder type;
};

}

using ::boost::asio::buffer;

}}



#endif /* INCLUDE_BOOST_PROCESS_DETAIL_ASYNC_HPP_ */
