/*
 * HANDLE_api.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DETAIL_HANDLE_API_HPP_
#define BOOST_DETAIL_HANDLE_API_HPP_

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/config.hpp>

//HANDLE_FLAG_INHERIT HANDLE_FLAG_INHERIT


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )
using ::GetExitCodeProcess;
using ::ExitProcess;
using ::TerminateProcess;

#else

__declspec(dllimport) __declspec (noreturn) void WINAPI ExitProcess (UINT_ uExitCode);
__declspec(dllimport) int WINAPI TerminateProcess (HANDLE_ hProcess, UINT_ uExitCode);
__declspec(dllimport) int WINAPI GetExitCodeProcess (HANDLE_ hProcess, LPDWORD_ lpExitCode);

}


#endif

}

}
}



#endif /* BOOST_DETAIL_HANDLE_API_HPP_ */
