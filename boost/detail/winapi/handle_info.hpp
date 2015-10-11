/*
 * handleapi.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens Morgenstern
 */

#ifndef BOOST_DETAIL_HANDLEAPI_HPP_
#define BOOST_DETAIL_HANDLEAPI_HPP_

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

using ::GetHandleInformation;
using ::SetHandleInformation;

const DWORD_ handle_flag_inherit = HANDLE_FLAG_INHERIT;
const DWORD_ handle_flag_protect_from_close = HANDLE_FLAG_PROTECT_FROM_CLOSE;

#else

const DWORD_ handle_flag_inherit 			= 0x1;
const DWORD_ handle_flag_protect_from_close = 0x2;

__declspec(dllimport) int WINAPI GetHandleInformation (HANDLE_ hObject, DWORD_* lpdwFlags);
__declspec(dllimport) int WINAPI SetHandleInformation (HANDLE_ hObject, DWORD_ dwMask, DWORD_ dwFlags);

}


#endif

}

}
}



#endif /* BOOST_DETAIL_HANDLEAPI_HPP_ */
