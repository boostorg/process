// Copyright (c) 2021 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_ERROR_IPP
#define BOOST_PROCESS_V2_IMPL_ERROR_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/error.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace error
{

namespace detail
{

// can be replaced with filesystem::codecvt_error_category in boost
struct utf8_category final : public error_category
{
    utf8_category() : error_category(0xDAEDu) {}

    const char* name() const noexcept
    {
        return "process.v2.utf8";
    }
    std::string message(int value) const
    {
        switch (static_cast<utf8_conv_error>(value))
        {
            case utf8_conv_error::insufficient_buffer:
                return "A supplied buffer size was not large enough";
            case utf8_conv_error::invalid_character:
                return "Invalid characters were found in a string.";
            default:
                return "process.v2.utf8 error";
        }
    }
};


} // namespace detail

BOOST_PROCESS_V2_DECL const error_category& get_utf8_category()
{
    static detail::utf8_category instance;
    return instance;
}

}

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_IMPL_ERROR_IPP
