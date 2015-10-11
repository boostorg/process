/*
 * startf.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DETAIL_WINAPI_STARTF_HPP_
#define BOOST_DETAIL_WINAPI_STARTF_HPP_

#include <boost/detail/winapi/basic_types.hpp>


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )

const DWORD_ startf_useshowwindow 		= STARTF_USESHOWWINDOW 	;
const DWORD_ startf_usesize 			= STARTF_USESIZE 		;
const DWORD_ startf_useposition 		= STARTF_USEPOSITION 	;
const DWORD_ startf_usecountchars 		= STARTF_USECOUNTCHARS 	;
const DWORD_ startf_usefillattribute 	= STARTF_USEFILLATTRIBUTE;
const DWORD_ startf_runfullscreen		= STARTF_RUNFULLSCREEN	;
const DWORD_ startf_forceonfeedback 	= STARTF_FORCEONFEEDBACK ;
const DWORD_ startf_forceofffeedback 	= STARTF_FORCEOFFFEEDBACK;
const DWORD_ startf_usestdhandles 		= STARTF_USESTDHANDLES 	;
const DWORD_ startf_usehotkey 			= STARTF_USEHOTKEY 		;
const DWORD_ startf_titleislinkname 	= STARTF_TITLEISLINKNAME ;
const DWORD_ startf_titleisappid 		= STARTF_TITLEISAPPID 	;
const DWORD_ startf_preventpinning		= STARTF_PREVENTPINNING	;

#else

const DWORD_ startf_useshowwindow 		= 0x00000001;
const DWORD_ startf_usesize 			= 0x00000002;
const DWORD_ startf_useposition 		= 0x00000004;
const DWORD_ startf_usecountchars 		= 0x00000008;
const DWORD_ startf_usefillattribute 	= 0x00000010;
const DWORD_ startf_runfullscreen		= 0x00000020;
const DWORD_ startf_forceonfeedback 	= 0x00000040;
const DWORD_ startf_forceofffeedback 	= 0x00000080;
const DWORD_ startf_usestdhandles 		= 0x00000100;
const DWORD_ startf_usehotkey 			= 0x00000200;
const DWORD_ startf_titleislinkname 	= 0x00000800;
const DWORD_ startf_titleisappid 		= 0x00001000;
const DWORD_ startf_preventpinning		= 0x00002000;
}


#endif

}

}
}

//STARTF_USESTDHANDLES



#endif /* BOOST_DETAIL_WINAPI_STARTF_HPP_ */
