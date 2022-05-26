// Copyright (c) 2021 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_ERROR_HPP
#define BOOST_PROCESS_V2_ERROR_HPP

#include <boost/process/v2/detail/config.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE

namespace error
{

enum utf8_conv_error
{
    insufficient_buffer = 1,
    invalid_character,
};

extern BOOST_PROCESS_V2_DECL const error_category& get_utf8_category();
static const error_category& utf8_category = get_utf8_category();
}

BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)

#include <boost/process/v2/impl/error.ipp>

#endif

#endif //BOOST_PROCESS_V2_ERROR_HPP
