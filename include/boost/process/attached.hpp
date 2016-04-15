// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_DETAIL_LAUNCH_MODE_HPP_
#define BOOST_PROCESS_DETAIL_LAUNCH_MODE_HPP_

#include <boost/process/detail/config.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/attached.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/attached.hpp>
#endif


namespace boost { namespace process { namespace detail {

constexpr api::attached attached;
}

using ::boost::process::detail::attached;

}}



#endif /* BOOST_PROCESS_DETAIL_HANDLER_HPP_ */
