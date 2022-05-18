// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_CONFIG_HPP
#define BOOST_PROCESS_V2_DETAIL_CONFIG_HPP

#if defined(BOOST_PROCESS_V2_STANDALONE)

#include <asio/detail/config.hpp>
#include <system_error>

#if defined(ASIO_WINDOWS)
#define BOOST_PROCESS_V2_WINDOWS 1
#endif

#if defined(ASIO_HAS_UNISTD_H)
#define BOOST_PROCESS_V2_POSIX 1
#endif

#define BOOST_PROCESS_V2_BEGIN_NAMESPACE namespace process_v2 {
#define BOOST_PROCESS_V2_END_NAMESPACE   }

#else
#include <boost/asio/detail/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_category.hpp>
#include <boost/system/system_error.hpp>

#if defined(BOOST_ASIO_WINDOWS)
#define BOOST_PROCESS_V2_WINDOWS 1
#endif

#if defined(BOOST_ASIO_HAS_UNISTD_H)
#define BOOST_PROCESS_V2_POSIX 1
#endif

#define BOOST_PROCESS_V2_BEGIN_NAMESPACE \
namespace boost { namespace process { namespace v2 {

#define BOOST_PROCESS_V2_END_NAMESPACE  } } }

#endif

#if !defined(BOOST_PROCESS_HAS_CHAR8_T)
# if (__cplusplus >= 202002)
#  define BOOST_PROCESS_HAS_CHAR8_T 1
# endif
#endif


BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_STANDALONE)

using std::error_code ;
using std::error_category ;
using std::system_category ;
using std::system_error ;

#else

using boost::system::error_code ;
using boost::system::error_category ;
using boost::system::system_category ;
using boost::system::system_error ;

#endif

BOOST_PROCESS_V2_END_NAMESPACE

#ifndef BOOST_PROCESS_V2_HEADER_ONLY
# ifndef BOOST_PROCESS_V2_SEPARATE_COMPILATION
#   define BOOST_PROCESS_V2_HEADER_ONLY 1
# endif
#endif

#if BOOST_PROCESS_V2_DOXYGEN
# define BOOST_PROCESS_V2_DECL
#elif defined(BOOST_PROCESS_V2_HEADER_ONLY)
# define BOOST_PROCESS_V2_DECL inline
#else
# define BOOST_PROCESS_V2_DECL
#endif

#endif //BOOST_PROCESS_V2_DETAIL_CONFIG_HPP
