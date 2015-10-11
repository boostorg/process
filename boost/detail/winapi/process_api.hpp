/*
 * HANDLE_api.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens Morgenstern
 */

#ifndef BOOST_DETAIL_PROCESS_API_HPP_
#define BOOST_DETAIL_PROCESS_API_HPP_

#include <boost/detail/winapi/config.hpp>
#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/security.hpp>
#include <boost/detail/winapi/process_info.hpp>

namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )
//|| defined( CreateProcess )
using ::GetExitCodeProcess;
using ::ExitProcess;
using ::TerminateProcess;
using ::CreateProcessA;
using ::CreateProcessW;

#else

__declspec(dllimport) __declspec (noreturn) void WINAPI ExitProcess (unsigned int uExitCode);
__declspec(dllimport) int WINAPI TerminateProcess 	(HANDLE_ hProcess, unsigned int uExitCode);
__declspec(dllimport) int WINAPI GetExitCodeProcess (HANDLE_ hProcess, DWORD_* lpExitCode);

__declspec(dllimport) int WINAPI CreateProcessA (LPCSTR_ lpApplicationName,  LPSTR_ lpCommandLine, LPSECURITY_ATTRIBUTES_ lpProcessAttributes, LPSECURITY_ATTRIBUTES_ lpThreadAttributes, int bInheritHandles, DWORD_ dwCreationFlags, LPVOID_ lpEnvironment, LPCSTR_ lpCurrentDirectory,   STARTUPINFOA_* lpStartupInfo, PROCESS_INFORMATION_* lpProcessInformation);
__declspec(dllimport) int WINAPI CreateProcessW (LPCWSTR_ lpApplicationName, LPWSTR_ lpCommandLine, LPSECURITY_ATTRIBUTES_ lpProcessAttributes, LPSECURITY_ATTRIBUTES_ lpThreadAttributes, int bInheritHandles, DWORD_ dwCreationFlags, LPVOID_ lpEnvironment, LPCWSTR_ lpCurrentDirectory, STARTUPINFOW_* lpStartupInfo, PROCESS_INFORMATION_* lpProcessInformation);



#if defined(UNICODE) && !defined(CreateProcess)
inline static int CreateProcess (LPCWSTR_ lpApplicationName, LPWSTR_ lpCommandLine, LPSECURITY_ATTRIBUTES_ lpProcessAttributes, LPSECURITY_ATTRIBUTES_ lpThreadAttributes, int bInheritHandles, DWORD_ dwCreationFlags, LPVOID_ lpEnvironment, LPCWSTR_ lpCurrentDirectory, STARTUPINFOW_* lpStartupInfo, PROCESS_INFORMATION_* lpProcessInformation)
{
	return CreateProcessW (lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,  lpStartupInfo,  lpProcessInformation);
}

#elif !defined(CreateProcess)
inline static int CreateProcess (LPCSTR_ lpApplicationName,  LPSTR_ lpCommandLine, LPSECURITY_ATTRIBUTES_ lpProcessAttributes, LPSECURITY_ATTRIBUTES_ lpThreadAttributes, int bInheritHandles, DWORD_ dwCreationFlags, LPVOID_ lpEnvironment, LPCSTR_ lpCurrentDirectory,   STARTUPINFOA_* lpStartupInfo, PROCESS_INFORMATION_* lpProcessInformation)
{
	return CreateProcessA (lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,  lpStartupInfo,  lpProcessInformation);
}
#endif //UNICODE
#endif //BOOST_USE_WINDOWS_H
}

}

}
}



#endif /* BOOST_DETAIL_HANDLE_API_HPP_ */
