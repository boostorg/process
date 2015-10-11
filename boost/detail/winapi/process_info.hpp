/*
 * process_info.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DETAIL_WINAPI_PROCESS_INFO_HPP_
#define BOOST_DETAIL_WINAPI_PROCESS_INFO_HPP_

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/config.hpp>
#include <boost/detail/winapi/handles.hpp>


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )
typedef ::PROCESS_INFORMATION PROCESS_INFORMATION_;
typedef ::STARTUPINFOA STARTUPINFOA_;
typedef ::STARTUPINFOW STARTUPINFOW_;

#else

struct PROCESS_INFORMATION_
{
    HANDLE_ hProcess;
    HANDLE_ hThread;
    DWORD_ dwProcessId;
    DWORD_ dwThreadId;
 };


struct STARTUPINFOA_ {
  DWORD_ cb;
  LPSTR_ lpReserved;
  LPSTR_ lpDesktop;
  LPSTR_ lpTitle;
  DWORD_ dwX;
  DWORD_ dwY;
  DWORD_ dwXSize;
  DWORD_ dwYSize;
  DWORD_ dwXCountChars;
  DWORD_ dwYCountChars;
  DWORD_ dwFillAttribute;
  DWORD_ dwFlags;
  WORD_ wShowWindow;
  WORD_ cbReserved2;
  BYTE_ *lpReserved2;
  HANDLE_ hStdInput;
  HANDLE_ hStdOutput;
  HANDLE_ hStdError;
};

struct STARTUPINFOW_ {
  DWORD cb;
  LPWSTR_ lpReserved;
  LPWSTR_ lpDesktop;
  LPWSTR_ lpTitle;
  DWORD_ dwX;
  DWORD_ dwY;
  DWORD_ dwXSize;
  DWORD_ dwYSize;
  DWORD_ dwXCountChars;
  DWORD_ dwYCountChars;
  DWORD_ dwFillAttribute;
  DWORD_ dwFlags;
  WORD_ wShowWindow;
  WORD_ cbReserved2;
  BYTE_ *lpReserved2;
  HANDLE_ hStdInput;
  HANDLE_ hStdOutput;
  HANDLE_ hStdError;
} ;


#if defined(UNICODE) || defined(_UNICODE)
typedef STARTUPINFOW_ STARTUPINFO_;
#else
typedef STARTUPINFOA_ STARTUPINFO_;
#endif

#if defined( BOOST_USE_WINDOWS_H )
typedef ::STARTUPINFOEX STARTUPINFOEX_;

#else

struct PROC_THREAD_ATTRIBUTE_LIST_ {};


struct STARTUPINFOEX_ {
  STARTUPINFO_                 StartupInfo;
  PROC_THREAD_ATTRIBUTE_LIST_* lpAttributeList;
};

#endif
#endif
}
}}}

#endif /* BOOST_DETAIL_WINAPI_PROCESS_INFO_HPP_ */
