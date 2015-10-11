/*
 * shell_api.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DETAIL_WINAPI_SHELL_API_HPP_
#define BOOST_DETAIL_WINAPI_SHELL_API_HPP_




#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/config.hpp>


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )

const DWORD_ shgfi_icon 				= SHGFI_ICON 			;
const DWORD_ shgfi_displayname 			= SHGFI_DISPLAYNAME 	;
const DWORD_ shgfi_typename 			= SHGFI_TYPENAME 		;
const DWORD_ shgfi_attributes 			= SHGFI_ATTRIBUTES 		;
const DWORD_ shgfi_iconlocation 		= SHGFI_ICONLOCATION 	;
const DWORD_ shgfi_exetype 				= SHGFI_EXETYPE 		;
const DWORD_ shgfi_sysiconindex 		= SHGFI_SYSICONINDEX 	;
const DWORD_ shgfi_linkoverlay 			= SHGFI_LINKOVERLAY 	;
const DWORD_ shgfi_selected 			= SHGFI_SELECTED 		;
const DWORD_ shgfi_attr_specified 		= SHGFI_ATTR_SPECIFIED 	;
const DWORD_ shgfi_largeicon 			= SHGFI_LARGEICON 		;
const DWORD_ shgfi_smallicon 			= SHGFI_SMALLICON 		;
const DWORD_ shgfi_openicon 			= SHGFI_OPENICON 		;
const DWORD_ shgfi_shelliconsize 		= SHGFI_SHELLICONSIZE 	;
const DWORD_ shgfi_pidl 				= SHGFI_PIDL 			;
const DWORD_ shgfi_usefileattributes	= SHGFI_USEFILEATTRIBUTES;
const DWORD_ shgfi_addoverlays 			= SHGFI_ADDOVERLAYS 	;
const DWORD_ shgfi_overlayindex 		= SHGFI_OVERLAYINDEX 	;
const DWORD_ max_path = MAX_PATH;


using ::SHGetFileInfoA;
using ::SHGetFileInfoW;

typedef ::SHFILEINFOA SHFILEINFOA_;
typedef ::SHFILEINFOW SHFILEINFOW_;

#else

struct ICON_ {};
typedef ICON_ *HICON_;

const DWORD_ max_path = 260;

struct SHFILEINFOA_ {
  HICON_ hIcon;
  int iIcon;
  DWORD_ dwAttributes;
  CHAR_  szDisplayName[260];
  CHAR_  szTypeName[80];
} ;

struct SHFILEINFOW_ {
  HICON_ hIcon;
  int iIcon;
  DWORD_ dwAttributes;
  WCHAR_ szDisplayName[260];
  WCHAR_ szTypeName[80];
};


const DWORD_ shgfi_icon 			 =	0x000000100;
const DWORD_ shgfi_displayname 		 =	0x000000200;
const DWORD_ shgfi_typename 		 =	0x000000400;
const DWORD_ shgfi_attributes 		 =	0x000000800;
const DWORD_ shgfi_iconlocation 	 =	0x000001000;
const DWORD_ shgfi_exetype 			 =	0x000002000;
const DWORD_ shgfi_sysiconindex 	 =	0x000004000;
const DWORD_ shgfi_linkoverlay 		 =	0x000008000;
const DWORD_ shgfi_selected 		 =	0x000010000;
const DWORD_ shgfi_attr_specified 	 =	0x000020000;
const DWORD_ shgfi_largeicon 		 =	0x000000000;
const DWORD_ shgfi_smallicon 		 =	0x000000001;
const DWORD_ shgfi_openicon 		 =	0x000000002;
const DWORD_ shgfi_shelliconsize 	 =	0x000000004;
const DWORD_ shgfi_pidl 			 =	0x000000008;
const DWORD_ shgfi_usefileattributes = 	0x000000010;
const DWORD_ shgfi_addoverlays 		 =	0x000000020;
const DWORD_ shgfi_overlayindex 	 =	0x000000040;

__declspec(dllimport) DWORD_* WINAPI SHGetFileInfoA (LPCSTR_ pszPath,  DWORD_ dwFileAttributes, SHFILEINFOA_ *psfinsigned, unsigned int cbFileInfons, unsigned int uFlags);
__declspec(dllimport) DWORD_* WINAPI SHGetFileInfoW (LPCWSTR_ pszPath, DWORD_ dwFileAttributes, SHFILEINFOW_ *psfinsigned, unsigned int cbFileInfons, unsigned int uFlags);


}


#endif


#if defined(UNICODE) || definde(_UNICODE)
typedef SHFILEINFOW_ SHFILEINFO_;
inline DWORD_* SHGetFileInfo (LPCWSTR_ pszPath, DWORD_ dwFileAttributes, SHFILEINFOW_ *psfinsigned, unsigned int cbFileInfons, unsigned int uFlags)
{
	return SHGetFileInfoW(pszPath,  dwFileAttributes, psfinsigned, cbFileInfons, uFlags);
}


#else
typedef SHFILEINFOA_ SHFILEINFO_;
inline DWORD_* SHGetFileInfo (LPCSTR_ pszPath,  DWORD_ dwFileAttributes, SHFILEINFOA_ *psfinsigned, unsigned int cbFileInfons, unsigned int uFlags)
{
	return SHGetFileInfoA (pszPath,  dwFileAttributes, psfinsigned, cbFileInfons, uFlags);
}




#endif

}

}
}



#endif /* BOOST_DETAIL_WINAPI_SHELL_API_HPP_ */
