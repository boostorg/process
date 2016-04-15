// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/config.hpp
 *
 * Defines various macros.
 */

#ifndef BOOST_PROCESS_DETAIL_CONFIG_HPP
#define BOOST_PROCESS_DETAIL_CONFIG_HPP

#include <boost/config.hpp>
#include <system_error>
#include <boost/system/api_config.hpp>


#if !defined(BOOST_GCC_CXX11) || defined (_GLIBCXX_HAS_GTHREADS)
#define BOOST_PROCESS_USE_FUTURE
#endif

#if defined(BOOST_POSIX_API)
#include <errno.h>
#elif defined(BOOST_WINDOWS_API)
#include <boost/detail/winapi/get_last_error.hpp>
#else
#error "System API not supported by boost.process"
#endif

namespace boost { namespace process { namespace detail
{


#if defined(BOOST_POSIX_API)
namespace posix {namespace extensions {}}
namespace api = posix;

inline std::error_code get_last_error() noexcept
{
    return std::error_code(errno, std::system_category());
}
#elif defined(BOOST_WINDOWS_API)
namespace windows {namespace extensions {}}
namespace api = windows;

inline std::error_code get_last_error() noexcept
{
    return std::error_code(::boost::detail::winapi::GetLastError(), std::system_category());
}
#endif

inline void throw_last_error(const std::string & msg)
{
    throw std::system_error(get_last_error(), msg);
}

inline void throw_last_error()
{
    throw std::system_error(get_last_error());
}

}

//#if defined(BOOST_POSIX_API)
//namespace posix   = boost::process::detail::posix  ::extensions;
//#elif defined(BOOST_WINDOWS_API)
//namespace windows = boost::process::detail::windows::extensions;
//#endif
//

template<typename Char> constexpr static Char null_char();
template<> constexpr char     null_char<char>     (){return   '\0';}
template<> constexpr wchar_t  null_char<wchar_t>  (){return  L'\0';}
template<> constexpr char16_t null_char<char16_t> (){return  u'\0';}
template<> constexpr char32_t null_char<char32_t> (){return  U'\0';}

template<typename Char> constexpr static Char equal_sign();
template<> constexpr char     equal_sign<char>    () {return  '='; }
template<> constexpr wchar_t  equal_sign<wchar_t> () {return L'='; }
template<> constexpr char16_t equal_sign<char16_t>() {return u'='; }
template<> constexpr char32_t equal_sign<char32_t>() {return U'='; }

}}
#endif
