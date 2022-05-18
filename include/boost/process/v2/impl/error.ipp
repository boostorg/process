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
struct codecvt_category : public error_category
{
    codecvt_category() : error_category(0xDAEDu) {}

    const char* name() const noexcept
    {
        return "process.v2.codecvt";
    }
    std::string message(int value) const
    {
        if (value == std::codecvt_base::ok)
            return "conversion completed without error.";
        else if (value == std::codecvt_base::partial)
            return "not enough space in the output buffer or unexpected end of source buffer";
        else if (value == std::codecvt_base::error)
            return "encountered a character that could not be converted";
        else if (value == std::codecvt_base::noconv)
            return "this facet is non-converting, no output written";
        return "process.v2.codecvt error";
    }
};


} // namespace detail

const error_category& get_codecvt_category()
{
    static detail::codecvt_category instance;
    return instance;
}

}

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_IMPL_ERROR_IPP
