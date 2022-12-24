// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/ext/exe.hpp>
#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ext)

BOOST_AUTO_TEST_CASE(test_exe)
{
    namespace bp2 = boost::process::v2;
    auto pth = bp2::ext::exe(bp2::current_pid());
    BOOST_CHECK(!pth.empty());
}


BOOST_AUTO_TEST_CASE(test_child_exe)
{
    using boost::unit_test::framework::master_test_suite;
    const auto pth =  master_test_suite().argv[1];
    namespace bp2 = boost::process::v2;

    boost::asio::io_context ctx;

    bp2::process proc(ctx, pth, {"sleep", "10000"});
    BOOST_CHECK_EQUAL(bp2::ext::exe(proc.handle()), pth);
}

BOOST_AUTO_TEST_SUITE_END()
