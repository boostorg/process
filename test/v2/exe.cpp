// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/ext/exe.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_exe)
{
    namespace bp2 = boost::process::v2;
    auto pth = bp2::ext::executable(bp2::current_pid()).string();
    BOOST_CHECK_NE(pth.empty(), true);
}

