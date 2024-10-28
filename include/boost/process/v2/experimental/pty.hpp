//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_VS_EXPERIMENTAL_PTY_HPP
#define BOOST_PROCESS_VS_EXPERIMENTAL_PTY_HPP

#include <boost/process/v2/experimental/basic_pty.hpp>

#if !defined(BOOST_PROCESS_V2_WINDOWS) || (NTDDI_VERSION >= 0x0A000006)

BOOST_PROCESS_V2_BEGIN_NAMESPACE
namespace experimental
{

typedef basic_pty<> pty;

}
BOOST_PROCESS_V2_END_NAMESPACE

#endif // windows version
#endif //BOOST_PROCESS_VS_EXPERIMENTAL_PTY_HPP
