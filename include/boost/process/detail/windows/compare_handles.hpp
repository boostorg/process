// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_WINDOWS_COMPARE_HANDLES_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_COMPARE_HANDLES_HPP_

#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/dll.hpp>
#include <boost/process/detail/config.hpp>


namespace boost { namespace process { namespace detail { namespace windows {


#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN10
inline bool compare_handles(boost::detail::winapi::HANDLE_ lhs, boost::detail::winapi::HANDLE_ rhs)
{
    return ::boost::detail::winapi::CompareObjectHandles(lhs, rhs);
}

#else //ugly workaround

extern "C"
{

typedef enum _OBJECT_INFORMATION_CLASS_
{
    ObjectBasicInformation,
    ObjectNameInformation,
    ObjectTypeInformation,
    ObjectAllInformation,
    ObjectDataInformation
} OBJECT_INFORMATION_CLASS_;

typedef struct _LSA_UNICODE_STRING {
    ::boost::detail::winapi::USHORT_ Length;
    ::boost::detail::winapi::USHORT_ MaximumLength;
    ::boost::detail::winapi::WCHAR_* Buffer;
} UNICODE_STRING_;


typedef struct _OBJECT_NAME_INFORMATION
{
    UNICODE_STRING_          Name;
    ::boost::detail::winapi::WCHAR_ NameBuffer[0];

} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef ::boost::detail::winapi::DWORD_ (* nt_query_object_p)(
  ::boost::detail::winapi::HANDLE_                   Handle,
  OBJECT_INFORMATION_CLASS_                          ObjectInformationClass,
  ::boost::detail::winapi::PVOID_                    ObjectInformation,
  ::boost::detail::winapi::ULONG_                    ObjectInformationLength,
  ::boost::detail::winapi::PULONG_                   ReturnLength
);

}


inline bool compare_handles(boost::detail::winapi::HANDLE_ lhs, boost::detail::winapi::HANDLE_ rhs)
{
    static ::boost::detail::winapi::HMODULE_ h = ::boost::detail::winapi::get_module_handle("ntdll.dll");
    static nt_query_object_p f = reinterpret_cast<nt_query_object_p>(::boost::detail::winapi::get_proc_address(h, "NtQueryObject"));

    _OBJECT_NAME_INFORMATION lhs_info;
    _OBJECT_NAME_INFORMATION rhs_info;

    if ((*f)(lhs, ObjectNameInformation, &rhs_info, sizeof(_OBJECT_NAME_INFORMATION), nullptr))
        ::boost::process::detail::throw_last_error("NtQueryObject failed");

    if ((*f)(rhs, ObjectNameInformation, &rhs_info, sizeof(_OBJECT_NAME_INFORMATION), nullptr))
        ::boost::process::detail::throw_last_error("NtQueryObject failed");

    return std::equal(lhs_info.Name.Buffer, lhs_info.Name.Buffer + lhs_info.Name.Length,
                      rhs_info.Name.Buffer, rhs_info.Name.Buffer + rhs_info.Name.Length);
}




#endif

}}}}



#endif /* BOOST_PROCESS_DETAIL_WINDOWS_COMPARE_HANDLES_HPP_ */
