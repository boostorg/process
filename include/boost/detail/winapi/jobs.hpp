//  environment.hpp  --------------------------------------------------------------//

//  Copyright 2016 Klemens D. Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DETAIL_WINAPI_JOBS_HPP_
#define BOOST_DETAIL_WINAPI_JOBS_HPP_

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/access_rights.hpp>
#include <boost/detail/winapi/limits.hpp>
#include <boost/core/underlying_type.hpp>


#if !defined( BOOST_USE_WINDOWS_H )
extern "C"
{

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI CreateJobObjectA (
    ::_SECURITY_ATTRIBUTES* lpJobAttributes,
    boost::detail::winapi::LPCSTR_ lpName);

#endif

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI CreateJobObjectW (
    ::_SECURITY_ATTRIBUTES* lpJobAttributes,
    boost::detail::winapi::LPCWSTR_ lpName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::BOOL_ WINAPI AssignProcessToJobObject(
    boost::detail::winapi::HANDLE_ hJob,
    boost::detail::winapi::HANDLE_ hProcess
);

BOOST_SYMBOL_IMPORT boost::detail::winapi::BOOL_ WINAPI IsProcessInJob(
    boost::detail::winapi::HANDLE_ ProcessHandle,
    boost::detail::winapi::HANDLE_ JobHandle,
    boost::detail::winapi::PBOOL_  Result
);

BOOST_SYMBOL_IMPORT boost::detail::winapi::BOOL_ WINAPI TerminateJobObject(
    boost::detail::winapi::HANDLE_ hJob,
    boost::detail::winapi::UINT_   uExitCode
);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI OpenJobObjectA(
  boost::detail::winapi::DWORD_   dwDesiredAccess,
  boost::detail::winapi::BOOL_    bInheritHandles,
  boost::detail::winapi::LPCSTR_ lpName
);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI OpenJobObjectW(
  boost::detail::winapi::DWORD_   dwDesiredAccess,
  boost::detail::winapi::BOOL_    bInheritHandles,
  boost::detail::winapi::LPCWSTR_ lpName
);
}
#endif

namespace boost { namespace detail { namespace winapi {

#if defined ( BOOST_USE_WINDOWS_H )

const DWORD_ JOB_OBJECT_ALL_ACCESS_ = JOB_OBJECT_ALL_ACCESS;

#else

const DWORD_ JOB_OBJECT_ALL_ACCESS_ = (STANDARD_RIGHTS_REQUIRED_ | SYNCHRONIZE_ | 0x1F);

#endif

#if !defined( BOOST_NO_ANSI_APIS )

using ::CreateJobObjectA;
using ::OpenJobObjectA;

#endif


using ::CreateJobObjectW;
using ::OpenJobObjectW;

using ::AssignProcessToJobObject;
using ::IsProcessInJob;
using ::TerminateJobObject;


#if !defined( BOOST_NO_ANSI_APIS )

inline HANDLE_ WINAPI create_job_object (
    LPSECURITY_ATTRIBUTES_ lpJobAttributes,
    LPCSTR_ lpName)
{
    return CreateJobObjectA(reinterpret_cast<::_SECURITY_ATTRIBUTES*>(lpJobAttributes), lpName);
}

inline HANDLE_ open_job_object(
        DWORD_ dwDesiredAccess,
        BOOL_ bInheritHandles,
        LPCSTR_ lpName)
{
    return OpenJobObjectA(dwDesiredAccess, bInheritHandles, lpName);
}

#endif

inline HANDLE_ WINAPI create_job_object (
    LPSECURITY_ATTRIBUTES_ lpJobAttributes,
    LPCWSTR_ lpName)
{
    return CreateJobObjectW(reinterpret_cast<::_SECURITY_ATTRIBUTES*>(lpJobAttributes), lpName);
}

inline HANDLE_ open_job_object(
        DWORD_ dwDesiredAccess,
        BOOL_ bInheritHandles,
        LPCWSTR_ lpName)
{
    return OpenJobObjectW(dwDesiredAccess, bInheritHandles, lpName);
}


}}}


#endif /* BOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_ */
