//
// boost/process/v2/windows/impl/job_object_service.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Klemens D. Morgenstern (klemens dot morgenstern at gmx dot net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_PROCESS_V2_IMPL_GROUP_IPP
#define BOOST_PROCESS_V2_IMPL_GROUP_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/group.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if !defined(BOOST_PROCESS_V2_HEADER_ONLY)
template struct basic_group_impl<BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor, default_process_launcher>;
#endif

BOOST_PROCESS_V2_END_NAMESPACE


#endif //BOOST_PROCESS_V2_IMPL_GROUP_IPP