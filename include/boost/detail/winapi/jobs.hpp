//  environment.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DETAIL_WINAPI_JOBS_HPP_
#define BOOST_DETAIL_WINAPI_JOBS_HPP_

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/limits.hpp>


#if !defined( BOOST_USE_WINDOWS_H )
extern "C"
{

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI CreateJobObjectA (
    boost::detail::winapi::LPSECURITY_ATTRIBUTES_ lpJobAttributes,
    boost::detail::winapi::LPCSTR_ lpName);

#endif

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI CreateJobObjectW (
      boost::detail::winapi::LPSECURITY_ATTRIBUTES_ lpJobAttributes,
      boost::detail::winapi::LPCWSTR_ lpName);


}
#endif

namespace boost { namespace detail { namespace winapi {


#if !defined( BOOST_NO_ANSI_APIS )

using ::CreateJobObjectA;

#endif

using ::CreateJobObjectW;


#if !defined( BOOST_NO_ANSI_APIS )


HANDLE_ WINAPI create_job_object (
    LPSECURITY_ATTRIBUTES_ lpJobAttributes,
    LPCSTR_ lpName)
{
    return CreateJobObjectA(lpJobAttributes, lpName);
}

#endif

HANDLE_ WINAPI create_job_object (
    LPSECURITY_ATTRIBUTES_ lpJobAttributes,
    LPCWSTR_ lpName)
{
    return CreateJobObjectW(lpJobAttributes, lpName);
}




}}}


#endif /* BOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_ */
