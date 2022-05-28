// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// Test that header file is self-contained.
#include <boost/process/v2/process.hpp>

#include <boost/process/v2/environment.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/asio/io_context.hpp>



namespace bpv = boost::process::v2;

BOOST_AUTO_TEST_CASE(exit_code_sync)
{
    using boost::unit_test::framework::master_test_suite;
    const auto pth =  master_test_suite().argv[1];
    
    bpv::environment::set("BOOST_PROCESS_V2_TEST_SUBPROCESS", "test");
    boost::asio::io_context ctx;
    
    BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"exit-code", "0"}).wait(), 0);
    BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"exit-code", "1"}).wait(), 1);
    BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"exit-code", "2"}).wait(), 2);
    BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"exit-code", "42"}).wait(), 42);

}

BOOST_AUTO_TEST_CASE(exit_code_async)
{
    using boost::unit_test::framework::master_test_suite;
    const auto pth =  master_test_suite().argv[1];
    
    bpv::environment::set("BOOST_PROCESS_V2_TEST_SUBPROCESS", "test");
    boost::asio::io_context ctx;

    int called = 0;
    
    bpv::process proc1(ctx, pth, {"exit-code", "0"});
    bpv::process proc2(ctx, pth, {"exit-code", "1"});
    bpv::process proc3(ctx, pth, {"exit-code", "2"});
    bpv::process proc4(ctx, pth, {"exit-code", "42"});

    proc1.async_wait([&](bpv::error_code ec, int e) {BOOST_CHECK(!ec); called++; BOOST_CHECK_EQUAL(bpv::evaluate_exit_code(e), 0);});
    proc2.async_wait([&](bpv::error_code ec, int e) {BOOST_CHECK(!ec); called++; BOOST_CHECK_EQUAL(bpv::evaluate_exit_code(e), 1);});
    proc3.async_wait([&](bpv::error_code ec, int e) {BOOST_CHECK(!ec); called++; BOOST_CHECK_EQUAL(bpv::evaluate_exit_code(e), 2);});
    proc4.async_wait([&](bpv::error_code ec, int e) {BOOST_CHECK(!ec); called++; BOOST_CHECK_EQUAL(bpv::evaluate_exit_code(e), 42);});
    ctx.run();
    BOOST_CHECK_EQUAL(called, 4);
}




