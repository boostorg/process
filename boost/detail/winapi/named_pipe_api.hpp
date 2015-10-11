/*
 * namepd_pipe_api.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens Morgenstern
 */

#ifndef BOOST_DETAIL_WINAPI_NAMED_PIPE_API_HPP_
#define BOOST_DETAIL_WINAPI_NAMED_PIPE_API_HPP_

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/config.hpp>
#include <boost/detail/winapi/security.hpp>


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )
using ::ImpersonateNamedPipeClient;
using ::CreatePipe;
using ::ConnectNamedPipe;
using ::DisconnectNamedPipe;
using ::SetNamedPipeHandleState;
using ::PeekNamedPipe;
using ::TransactNamedPipe;
using ::CreateNamedPipeW;
using ::WaitNamedPipeW;
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
using ::GetNamedPipeClientComputerNameW;
#endif
using ::OVERLAPPED_;
using ::LPOVERLAPPED_;
#else

struct OVERLAPPED_ {
  ULONG_PTR_ Internal;
  ULONG_PTR_ InternalHigh;
  union {
    struct {
      DWORD_ Offset;
      DWORD_ OffsetHigh;
    } ;
    PVOID_  Pointer;
  } ;
  HANDLE_    hEvent;
};

typedef OVERLAPPED_ *LPOVERLAPPED_;

__declspec(dllimport) int     WINAPI ImpersonateNamedPipeClient (HANDLE_ hNamedPipe);
__declspec(dllimport) int     WINAPI CreatePipe (HANDLE_* hReadPipe, HANDLE_* hWritePipe, LPSECURITY_ATTRIBUTES_ lpPipeAttributes, DWORD_ nSize);
__declspec(dllimport) int     WINAPI ConnectNamedPipe (HANDLE_ hNamedPipe, LPOVERLAPPED_ lpOverlapped);
__declspec(dllimport) int     WINAPI DisconnectNamedPipe (HANDLE_ hNamedPipe);
__declspec(dllimport) int     WINAPI SetNamedPipeHandleState (HANDLE_ hNamedPipe, DWORD_* lpMode, DWORD_* lpMaxCollectionCount, DWORD_* lpCollectDataTimeout);
__declspec(dllimport) int     WINAPI PeekNamedPipe (HANDLE_ hNamedPipe, LPVOID_ lpBuffer, DWORD_ nBufferSize, DWORD_* lpBytesRead, DWORD_* lpTotalBytesAvail, DWORD_* lpBytesLeftThisMessage);
__declspec(dllimport) int     WINAPI TransactNamedPipe (HANDLE_ hNamedPipe, LPVOID_ lpInBuffer, DWORD_ nInBufferSize, LPVOID_ lpOutBuffer, DWORD_ nOutBufferSize, DWORD_* lpBytesRead, LPOVERLAPPED_ lpOverlapped);
__declspec(dllimport) HANDLE_ WINAPI CreateNamedPipeW (LPCWSTR_ lpName, DWORD_ dwOpenMode, DWORD_ dwPipeMode, DWORD_ nMaxInstances, DWORD_ nOutBufferSize, DWORD_ nInBufferSize, DWORD_ nDefaultTimeOut, LPSECURITY_ATTRIBUTES_ lpSecurityAttributes);
__declspec(dllimport) int 	   WINAPI WaitNamedPipeW (LPCWSTR_ lpNamedPipeName, DWORD_ nTimeOut);

#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
  WINBASEAPI int WINAPI GetNamedPipeClientComputerNameW (HANDLE_ Pipe, LPWSTR ClientComputerName, ULONG ClientComputerNameLength);
#endif

#if defined(UNICODE)


inline HANDLE_ CreateNamedPipe(LPCWSTR_ lpName, DWORD_ dwOpenMode, DWORD_ dwPipeMode, DWORD_ nMaxInstances, DWORD_ nOutBufferSize, DWORD_ nInBufferSize, DWORD_ nDefaultTimeOut, LPSECURITY_ATTRIBUTES_ lpSecurityAttributes)
{
	return CreateNamedPipeW(lpName, dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize, nInBufferSize, nDefaultTimeOut, lpSecurityAttributes);
}

inline int WaitNamedPipe(LPCWSTR_ lpNamedPipeName, DWORD_ nTimeOut)
{
	return WaitNamedPipeW(lpNamedPipeName, nTimeOut);
}
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6

inline int GetNamedPipeClientComputerName(HANDLE_ Pipe, LPWSTR ClientComputerName, ULONG ClientComputerNameLength)
{
	return GetNamedPipeClientComputerNameW(Pipe, ClientComputerName, ClientComputerNameLength);
}
#endif
#endif
#endif
}



}

}
}


#endif /* BOOST_DETAIL_WINAPI_NAMED_PIPE_API_HPP_ */
