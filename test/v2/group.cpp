// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// Test that header file is self-contained.

#include <boost/test/unit_test.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/process/v2/group.hpp>
#include <boost/process/v2.hpp>

namespace bp2 = boost::process::v2;
namespace bpw = boost::process::v2::windows;
namespace bpd = boost::process::v2::detail;
namespace asio = boost::asio;

BOOST_AUTO_TEST_CASE(wait_one)
{
    using boost::unit_test::framework::master_test_suite;
    asio::io_context ctx;
    const auto pth = master_test_suite().argv[1];

    bp2::error_code ec;
    bp2::group grp{ctx};

    BOOST_CHECK_MESSAGE(!ec, ec.message());

    auto pid1 = grp.emplace(pth, {"sleep", "100"});
    auto pid2 = grp.emplace(pth, {"sleep", "300"});
    auto pid3 = grp.emplace(pth, {"sleep", "500"});

    auto res1 = grp.wait_one(ec); BOOST_CHECK_MESSAGE(!ec, ec.message());
    auto res2 = grp.wait_one(ec); BOOST_CHECK_MESSAGE(!ec, ec.message());
    auto res3 = grp.wait_one(ec); BOOST_CHECK_MESSAGE(!ec, ec.message());
    
    
    grp.wait_all(); //
    
    BOOST_CHECK_EQUAL(res1.exit_code, 0);
    BOOST_CHECK_EQUAL(res2.exit_code, 0);
    BOOST_CHECK_EQUAL(res3.exit_code, 0);

    BOOST_CHECK_EQUAL(res1.pid, pid1);
    BOOST_CHECK_EQUAL(res2.pid, pid2);
    BOOST_CHECK_EQUAL(res3.pid, pid3);

    BOOST_CHECK(grp.is_open());
    BOOST_CHECK_MESSAGE(!ec, ec.message());
}

BOOST_AUTO_TEST_CASE(wait_all)
{
    using boost::unit_test::framework::master_test_suite;
    asio::io_context ctx;
    const auto pth = master_test_suite().argv[1];

    bp2::group grp{ctx};


    auto pid1 = grp.emplace(pth, {"sleep", "10"});
    auto pid2 = grp.emplace(pth, {"sleep", "30"});
    auto pid3 = grp.emplace(pth, {"sleep", "50"});

    grp.wait_all(); //
    
    BOOST_CHECK(grp.is_open());
}

BOOST_AUTO_TEST_CASE(terminate_)
{
    using boost::unit_test::framework::master_test_suite;
    asio::io_context ctx;
    const auto pth = master_test_suite().argv[1];

    bp2::error_code ec;
    bp2::group grp{ctx};

    BOOST_CHECK_MESSAGE(!ec, ec.message());

    const auto pid1 = grp.emplace(pth, {"sleep", "10000000"});
    const auto pid2 = grp.emplace(pth, {"sleep", "10000000"});
    const auto pid3 = grp.emplace(pth, {"sleep", "10000000"});

    const auto start = std::chrono::steady_clock::now();

    grp.terminate();
    grp.wait_all();

    const auto end = std::chrono::steady_clock::now();
    BOOST_CHECK((start + std::chrono::milliseconds(5000)) > end);
    
    BOOST_CHECK(grp.is_open());
    BOOST_CHECK_MESSAGE(!ec, ec.message());
}

BOOST_AUTO_TEST_CASE(async_wait_one)
{
    using boost::unit_test::framework::master_test_suite;
    asio::io_context ctx;
    const auto pth = master_test_suite().argv[1];

    bp2::error_code ec;
    bp2::group grp{ctx};

    BOOST_CHECK_MESSAGE(!ec, ec.message());

    auto pid1 = grp.emplace(pth, {"sleep", "100"});
    auto pid2 = grp.emplace(pth, {"sleep", "300"});
    auto pid3 = grp.emplace(pth, {"sleep", "500"});

    std::vector<bp2::pid_type> res;

    grp.async_wait_one(
        [&](bp2::error_code ec, bp2::single_process_exit sp) 
        {
            res.push_back(sp.pid); 
            BOOST_CHECK_EQUAL(sp.exit_code, 0u);
            BOOST_CHECK_MESSAGE(!ec, ec.message());
        });
    grp.async_wait_one(
        [&](bp2::error_code ec, bp2::single_process_exit sp) 
        {
            res.push_back(sp.pid); 
            BOOST_CHECK_EQUAL(sp.exit_code, 0u);
            BOOST_CHECK_MESSAGE(!ec, ec.message());
        });
    grp.async_wait_one(
        [&](bp2::error_code ec, bp2::single_process_exit sp) 
        {
            res.push_back(sp.pid); 
            BOOST_CHECK_EQUAL(sp.exit_code, 0u);
            BOOST_CHECK_MESSAGE(!ec, ec.message());
        });
    
    BOOST_CHECK_GE(ctx.run(), 0u);

    BOOST_CHECK_EQUAL(res.at(0), pid1);
    BOOST_CHECK_EQUAL(res.at(1), pid2);
    BOOST_CHECK_EQUAL(res.at(2), pid3);


    BOOST_CHECK(grp.is_open());
    BOOST_CHECK_MESSAGE(!ec, ec.message());
}

BOOST_AUTO_TEST_CASE(async_wait_all)
{
    using boost::unit_test::framework::master_test_suite;
    asio::io_context ctx;
    const auto pth = master_test_suite().argv[1];

    bp2::error_code ec;
    bp2::group grp{ctx};

    BOOST_CHECK_MESSAGE(!ec, ec.message());

    auto pid1 = grp.emplace(pth, {"sleep", "10"});
    auto pid2 = grp.emplace(pth, {"sleep", "30"});
    auto pid3 = grp.emplace(pth, {"sleep", "50"});

    grp.async_wait_all(
        [](bp2::error_code ec)
        {
            BOOST_CHECK_MESSAGE(!ec, ec.message());
        }); 

    ctx.run();


    BOOST_CHECK(grp.is_open());
    BOOST_CHECK_MESSAGE(!ec, ec.message());
}
