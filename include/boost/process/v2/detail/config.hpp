// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_CONFIG_HPP
#define BOOST_PROCESS_V2_DETAIL_CONFIG_HPP

#if defined(BOOST_PROCESS_V2_STANDALONE)

#include <asio/detail/config.hpp>
#include <system_error>
#include <filesystem>
#include <string_view>
#include <iomanip>

#if defined(ASIO_WINDOWS)
#define BOOST_PROCESS_V2_WINDOWS 1

// Windows: suppress definition of "min" and "max" macros.
#if !defined(NOMINMAX)
# define NOMINMAX 1
#endif
#endif

#if defined(ASIO_HAS_UNISTD_H)
#define BOOST_PROCESS_V2_POSIX 1
#endif

#define BOOST_PROCESS_V2_BEGIN_NAMESPACE namespace process_v2 {
#define BOOST_PROCESS_V2_END_NAMESPACE   }
#define BOOST_PROCESS_V2_NAMESPACE process_v2

#else
#include <boost/config.hpp>
#include <boost/io/quoted.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_category.hpp>
#include <boost/system/system_error.hpp>

#if defined(BOOST_WINDOWS_API)
#define BOOST_PROCESS_V2_WINDOWS 1

// Windows: suppress definition of "min" and "max" macros.
#if !defined(NOMINMAX)
# define NOMINMAX 1
#endif

#endif

#if defined(BOOST_POSIX_API)
#define BOOST_PROCESS_V2_POSIX 1
#endif

#if !defined(BOOST_PROCESS_V2_WINDOWS) && !defined(BOOST_POSIX_API)
#error Unsupported operating system
#endif

#if defined(BOOST_PROCESS_USE_STD_FS)
#include <filesystem>
#include <optional>
#else
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/optional.hpp>
#endif

#define BOOST_PROCESS_V2_BEGIN_NAMESPACE namespace boost { namespace process { namespace v2 {
#define BOOST_PROCESS_V2_END_NAMESPACE  } } }
#define BOOST_PROCESS_V2_NAMESPACE boost::process::v2

#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_STANDALONE)

using std::error_code ;
using std::error_category ;
using std::system_category ;
using std::system_error ;
namespace filesystem = std::filesystem;
using std::quoted;
using std::optional;

#else

using boost::system::error_code ;
using boost::system::error_category ;
using boost::system::system_category ;
using boost::system::system_error ;
using boost::io::quoted;
using boost::optional;

#ifdef BOOST_PROCESS_USE_STD_FS
namespace filesystem = std::filesystem;
#else
namespace filesystem = boost::filesystem;
#endif

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
