// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_IMPL_UTF8_HPP
#define BOOST_PROCESS_V2_DETAIL_IMPL_UTF8_HPP

#include <boost/process/v2/detail/utf8.hpp>
#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/last_error.hpp>
#include <boost/process/v2/error.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <Windows.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace detail
{

#if defined(BOOST_PROCESS_V2_WINDOWS)

inline void handle_error(error_code & ec)
{
    const auto err = ::GetLastError();
    switch (err)
    {
    case ERROR_INSUFFICIENT_BUFFER:
        ec.assign(error::insufficient_buffer, error::utf8_category);
        break;
    case ERROR_NO_UNICODE_TRANSLATION:
        ec.assign(error::invalid_character, error::utf8_category);
        break;
    default:
        ec.assign(err, system_category());
    }
}

std::size_t size_as_utf8(const wchar_t * in, std::size_t size, error_code & ec)
{
    auto res = WideCharToMultiByte(
                          CP_UTF8,                // CodePage,
                          0,                      // dwFlags,
                          in,                     // lpWideCharStr,
                          static_cast<int>(size), // cchWideChar,
                          nullptr,                // lpMultiByteStr,
                          0,                      // cbMultiByte,
                          nullptr,                // lpDefaultChar,
                          FALSE);                 // lpUsedDefaultChar
    
    if (res == 0u)
        handle_error(ec);
    return static_cast<std::size_t>(res);
}

std::size_t size_as_wide(const char * in, std::size_t size, error_code & ec)
{
    auto res = ::MultiByteToWideChar(
                          CP_UTF8,                // CodePage
                          0,                      // dwFlags
                          in,                     // lpMultiByteStr
                          static_cast<int>(size), // cbMultiByte
                          nullptr,                // lpWideCharStr
                          0);                     // cchWideChar
    if (res == 0u)
        handle_error(ec);

    return static_cast<std::size_t>(res);
}

std::size_t convert_to_utf8(const wchar_t *in, std::size_t size,  char * out, 
                            std::size_t max_size, error_code & ec)
{
    auto res = ::WideCharToMultiByte(
                    CP_UTF8,                    // CodePage
                    0,                          // dwFlags
                    in,                         // lpWideCharStr
                    static_cast<int>(size),     // cchWideChar
                    out,                        // lpMultiByteStr
                    static_cast<int>(max_size), // cbMultiByte
                    nullptr,                    // lpDefaultChar
                    FALSE);                     // lpUsedDefaultChar
    if (res == 0u)
        handle_error(ec);

    return static_cast<std::size_t>(res);
}

std::size_t convert_to_wide(const char *in, std::size_t size,  wchar_t * out, 
                            std::size_t max_size, error_code & ec)
{
    auto res = ::MultiByteToWideChar(
                          CP_UTF8,                     // CodePage
                          0,                           // dwFlags
                          in,                          // lpMultiByteStr
                          static_cast<int>(size),      // cbMultiByte
                          out,                         // lpWideCharStr
                          static_cast<int>(max_size)); // cchWideChar
    if (res == 0u)
        handle_error(ec);

    return static_cast<std::size_t>(res);
}

#else

#endif

}

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_DETAIL_IMPL_UTF8_HPP
