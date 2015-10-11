/*
 * environment.hpp
 *
 *  Created on: 11.10.2015
 *      Author: Klemens Morgenstern
 */

#ifndef BOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_
#define BOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_

#include <boost/detail/winapi/basic_types.hpp>


namespace boost
{
namespace detail
{
namespace winapi
{
extern "C" {

#if defined( BOOST_USE_WINDOWS_H )

const DWORD_ debug_process                    = DEBUG_PROCESS                   ;
const DWORD_ debug_only_this_process          = DEBUG_ONLY_THIS_PROCESS         ;
const DWORD_ create_suspended                 = CREATE_SUSPENDED                ;
const DWORD_ detached_process                 = DETACHED_PROCESS                ;
const DWORD_ create_new_console               = CREATE_NEW_CONSOLE              ;
const DWORD_ normal_priority_class            = NORMAL_PRIORITY_CLASS           ;
const DWORD_ idle_priority_class              = IDLE_PRIORITY_CLASS             ;
const DWORD_ high_priority_class              = HIGH_PRIORITY_CLASS             ;
const DWORD_ realtime_priority_class          = REALTIME_PRIORITY_CLASS         ;
const DWORD_ create_new_process_group         = CREATE_NEW_PROCESS_GROUP        ;
const DWORD_ create_unicode_environment       = CREATE_UNICODE_ENVIRONMENT      ;
const DWORD_ create_separate_wow_vdm          = CREATE_SEPARATE_WOW_VDM         ;
const DWORD_ create_shared_wow_vdm            = CREATE_SHARED_WOW_VDM           ;
const DWORD_ create_forcedos                  = CREATE_FORCEDOS                 ;
const DWORD_ below_normal_priority_class      = BELOW_NORMAL_PRIORITY_CLASS     ;
const DWORD_ above_normal_priority_class      = ABOVE_NORMAL_PRIORITY_CLASS     ;
const DWORD_ inherit_parent_affinity          = INHERIT_PARENT_AFFINITY         ;
const DWORD_ inherit_caller_priority          = INHERIT_CALLER_PRIORITY         ;
const DWORD_ create_protected_process         = CREATE_PROTECTED_PROCESS        ;
const DWORD_ extended_startupinfo_present     = EXTENDED_STARTUPINFO_PRESENT    ;
const DWORD_ process_mode_background_begin    = PROCESS_MODE_BACKGROUND_BEGIN   ;
const DWORD_ process_mode_background_end      = PROCESS_MODE_BACKGROUND_END     ;
const DWORD_ create_breakaway_from_job        = CREATE_BREAKAWAY_FROM_JOB       ;
const DWORD_ create_preserve_code_authz_level = CREATE_PRESERVE_CODE_AUTHZ_LEVEL;
const DWORD_ create_default_error_mode        = CREATE_DEFAULT_ERROR_MODE       ;
const DWORD_ create_no_window                 = CREATE_NO_WINDOW                ;
const DWORD_ profile_user                     = PROFILE_USER                    ;
const DWORD_ profile_kernel                   = PROFILE_KERNEL                  ;
const DWORD_ profile_server                   = PROFILE_SERVER                  ;
const DWORD_ create_ignore_system_default     = CREATE_IGNORE_SYSTEM_DEFAULT    ;

#else

const DWORD_ debug_process                    = 0x1        ;
const DWORD_ debug_only_this_process          = 0x2        ;
const DWORD_ create_suspended                 = 0x4        ;
const DWORD_ detached_process                 = 0x8        ;
const DWORD_ create_new_console               = 0x10       ;
const DWORD_ normal_priority_class            = 0x20       ;
const DWORD_ idle_priority_class              = 0x40       ;
const DWORD_ high_priority_class              = 0x80       ;
const DWORD_ realtime_priority_class          = 0x100      ;
const DWORD_ create_new_process_group         = 0x200      ;
const DWORD_ create_unicode_environment       = 0x400      ;
const DWORD_ create_separate_wow_vdm          = 0x800      ;
const DWORD_ create_shared_wow_vdm            = 0x1000     ;
const DWORD_ create_forcedos                  = 0x2000     ;
const DWORD_ below_normal_priority_class      = 0x4000     ;
const DWORD_ above_normal_priority_class      = 0x8000     ;
const DWORD_ inherit_parent_affinity          = 0x10000    ;
const DWORD_ inherit_caller_priority          = 0x20000    ;
const DWORD_ create_protected_process         = 0x40000    ;
const DWORD_ extended_startupinfo_present     = 0x80000    ;
const DWORD_ process_mode_background_begin    = 0x100000   ;
const DWORD_ process_mode_background_end      = 0x200000   ;
const DWORD_ create_breakaway_from_job        = 0x1000000  ;
const DWORD_ create_preserve_code_authz_level = 0x2000000  ;
const DWORD_ create_default_error_mode        = 0x4000000  ;
const DWORD_ create_no_window                 = 0x8000000  ;
const DWORD_ profile_user                     = 0x10000000 ;
const DWORD_ profile_kernel                   = 0x20000000 ;
const DWORD_ profile_server                   = 0x40000000 ;
const DWORD_ create_ignore_system_default     = 0x80000000 ;
}


#endif

}

}
}


#endif /* BOOST_DETAIL_WINAPI_ENVIRONMENT_HPP_ */
