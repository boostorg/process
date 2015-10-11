/*
 * string.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DETAIL_WINAPI_TCHAR_HPP_
#define BOOST_DETAIL_WINAPI_TCHAR_HPP_

#include <boost/detail/winapi/basic_types.hpp>

namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )

typedef ::TCHAR TCHAR_;

#else

#if defined(UNICODE)
typedef wchar_t TCHAR_;
#else
typedef char TCHAR_;
#endif
}


#endif

}

}
}



#endif /* BOOST_DETAIL_WINAPI_TCHAR_HPP_ */
