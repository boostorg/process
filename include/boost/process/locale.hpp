// Copyright (c) 2016 Klemens D. Morgenstern
// Copyright (c) 2008 Beman Dawes
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_LOCALE_HPP_
#define BOOST_PROCESS_LOCALE_HPP_

#include <boost/process/detail/config.hpp>

#if defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/locale.hpp>
#endif

#include <locale>

namespace boost
{
namespace process
{
namespace detail
{

//copied from boost.filesystem
inline std::locale default_locale()
{
# if defined(BOOST_WINDOWS_API)
  std::locale global_loc = std::locale();
  return std::locale(global_loc, new boost::process::detail::windows::windows_file_codecvt);
# elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) \
|| defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__)
  std::locale global_loc = std::locale();
  return std::locale(global_loc, new std::utf8_codecvt_facet);
# else  // Other POSIX
  // ISO C calls std::locale("") "the locale-specific native environment", and this
  // locale is the default for many POSIX-based operating systems such as Linux.
  return std::locale("");
# endif
}

inline std::locale& process_locale()
{
	static std::locale loc(default_locale());
    return loc;
}

}

typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;


inline const codecvt_type& codecvt()
{
  return std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(
		  	  detail::process_locale());
}

inline std::locale imbue(const std::locale& loc)
{
  std::locale temp(detail::process_locale());
  detail::process_locale() = loc;
  return temp;
}


}
}




#endif /* BOOST_PROCESS_LOCALE_HPP_ */
