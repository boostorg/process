// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/ext/cmd.hpp>
#include <boost/process/v2/ext/exe.hpp>
#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ext)

BOOST_AUTO_TEST_CASE(test_exe)
{
    using boost::unit_test::framework::master_test_suite;

    namespace bp2 = boost::process::v2;
    auto pth = bp2::ext::exe(bp2::current_pid());
    BOOST_CHECK(!pth.empty());
    BOOST_CHECK_EQUAL(master_test_suite().argv[0], pth.string());
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

BOOST_AUTO_TEST_CASE(cmd)
{
    using boost::unit_test::framework::master_test_suite;

    namespace bp2 = boost::process::v2;
    auto cmd = bp2::ext::cmd(bp2::current_pid());

    // the test framework drops a bunch of args.
    BOOST_CHECK_EQUAL(std::string(cmd.args()[0]), master_test_suite().argv[0]);

    auto cm = cmd.argv() + (cmd.argc() - master_test_suite().argc);
    for (auto i = 1; i < master_test_suite().argc; i++)
        BOOST_CHECK_EQUAL(std::string(cm[i]), master_test_suite().argv[i]);
}


BOOST_AUTO_TEST_CASE(cmd_exe)
{
    using boost::unit_test::framework::master_test_suite;
    const auto pth =  master_test_suite().argv[1];

    namespace bp2 = boost::process::v2;

    boost::asio::io_context ctx;
    std::vector<std::string> args = {"sleep", "10000", "moar", "args", "  to test "};
    bp2::process proc(ctx, pth, args);
    auto cm = bp2::ext::cmd(proc.handle());

    BOOST_CHECK_EQUAL(cm.args()[0], pth);

    BOOST_REQUIRE_EQUAL(cm.argc(), args.size() + 1);
    for (auto i = 0; i < args.size(); i++)
        BOOST_CHECK_EQUAL(cm.args()[i + 1], args[i]);

}


BOOST_AUTO_TEST_SUITE_END()
