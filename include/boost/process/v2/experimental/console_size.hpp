//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_V2_CONSOLE_SIZE_CONSOLE_SIZE_HPP
#define BOOST_PROCESS_V2_CONSOLE_SIZE_CONSOLE_SIZE_HPP

#include <boost/process/v2/detail/config.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE
namespace experimental
{

struct console_size_t
{
  unsigned short columns, rows;
};

}
BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_CONSOLE_SIZE_CONSOLE_SIZE_HPP
