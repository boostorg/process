/*
 * show_windows.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DETAIL_WINAPI_SHOW_WINDOWS_HPP_
#define BOOST_DETAIL_WINAPI_SHOW_WINDOWS_HPP_



#include <boost/detail/winapi/basic_types.hpp>


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )

const DWOD_ sw_hide 			= SW_HIDE 			 	;
const DWOD_ sw_shownormal 		= SW_SHOWNORMAL 		;
const DWOD_ sw_normal 			= SW_NORMAL 			;
const DWOD_ sw_showminimized 	= SW_SHOWMINIMIZED 	    ;
const DWOD_ sw_showmaximized	= SW_SHOWMAXIMIZED	 	;
const DWOD_ sw_maximize 		= SW_MAXIMIZE 		    ;
const DWOD_ sw_shownoactivate 	= SW_SHOWNOACTIVATE 	;
const DWOD_ sw_show 			= SW_SHOW 			 	;
const DWOD_ sw_minimize 		= SW_MINIMIZE 		 	;
const DWOD_ sw_showminnoactive  = SW_SHOWMINNOACTIVE  	;
const DWOD_ sw_showna 			= SW_SHOWNA 			;
const DWOD_ sw_restore 		 	= SW_RESTORE 		 	;
const DWOD_ sw_showdefault 	 	= SW_SHOWDEFAULT 	 	;
const DWOD_ sw_forceminimize 	= SW_FORCEMINIMIZE 	    ;
const DWOD_ sw_max 			 	= SW_MAX 			 	;
const DWOD_ hide_window 		= HIDE_WINDOW 		 	;
const DWOD_ show_openwindow 	= SHOW_OPENWINDOW 	 	;
const DWOD_ show_iconwindow 	= SHOW_ICONWINDOW 	 	;
const DWOD_ show_fullscreen 	= SHOW_FULLSCREEN 	 	;
const DWOD_ show_opennoactivate = SHOW_OPENNOACTIVATE 	;
const DWOD_ sw_parentclosing 	= SW_PARENTCLOSING 	    ;
const DWOD_ sw_otherzoom 		= SW_OTHERZOOM 		    ;
const DWOD_ sw_parentopening 	= SW_PARENTOPENING 	    ;
const DWOD_ sw_otherunzoom 	 	= SW_OTHERUNZOOM 	 	;



#else

const DWORD_ sw_hide 			 = 0 ;
const DWORD_ sw_shownormal 		 = 1 ;
const DWORD_ sw_normal 			 = 1 ;
const DWORD_ sw_showminimized 	 = 2 ;
const DWORD_ sw_showmaximized	 = 3 ;
const DWORD_ sw_maximize 		 = 3 ;
const DWORD_ sw_shownoactivate 	 = 4 ;
const DWORD_ sw_show 			 = 5 ;
const DWORD_ sw_minimize 		 = 6 ;
const DWORD_ sw_showminnoactive  = 7 ;
const DWORD_ sw_showna 			 = 8 ;
const DWORD_ sw_restore 		 = 9 ;
const DWORD_ sw_showdefault 	 = 10;
const DWORD_ sw_forceminimize 	 = 11;
const DWORD_ sw_max 			 = 11;
const DWORD_ hide_window 		 = 0 ;
const DWORD_ show_openwindow 	 = 1 ;
const DWORD_ show_iconwindow 	 = 2 ;
const DWORD_ show_fullscreen 	 = 3 ;
const DWORD_ show_opennoactivate = 4 ;
const DWORD_ sw_parentclosing 	 = 1 ;
const DWORD_ sw_otherzoom 		 = 2 ;
const DWORD_ sw_parentopening 	 = 3 ;
const DWORD_ sw_otherunzoom 	 = 4 ;


}


#endif

}

}
}

//STARTF_USESTDHANDLES




#ifndef NOSHOWWINDOW
#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_NORMAL 1
#define SW_SHOWMINIMIZED 2
#define SW_SHOWMAXIMIZED 3
#define SW_MAXIMIZE 3
#define SW_SHOWNOACTIVATE 4
#define SW_SHOW 5
#define SW_MINIMIZE 6
#define SW_SHOWMINNOACTIVE 7
#define SW_SHOWNA 8
#define SW_RESTORE 9
#define SW_SHOWDEFAULT 10
#define SW_FORCEMINIMIZE 11
#define SW_MAX 11



#endif /* BOOST_DETAIL_WINAPI_SHOW_WINDOWS_HPP_ */
