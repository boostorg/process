// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_VARIANT_HPP_
#define BOOST_PROCESS_VARIANT_HPP_

#include <boost/process/detail/config.hpp>
#if defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/variant.hpp>
#elif defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/variant.hpp>
#endif


namespace boost
{
namespace process
{

using ::boost::process::detail::api::handler_variant;


}
}



#endif /* INCLUDE_BOOST_PROCESS_IO_VARIANT_HPP_ */
