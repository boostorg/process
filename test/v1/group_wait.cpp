// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#define BOOST_TEST_MAIN
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/scope_exit.hpp>
#include <boost/process/v1/error.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/args.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/group.hpp>
#include <system_error>

#include <string>
#include <thread>
#include <istream>
#include <iostream>
#include <cstdlib>

namespace bp = boost::process::v1;



BOOST_AUTO_TEST_CASE(wait_group_test, *boost::unit_test::timeout(5))
{
    std::atomic<bool> done{false};
    std::thread thr{
        [&]
        {
            for (int i = 0; i < 100 && !done.load(); i++)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            BOOST_REQUIRE(done.load());
        }};

    BOOST_SCOPE_EXIT_ALL(&) {    
        done.store(true);
        thr.join();
    };

    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;
    bp::group g;


    bp::child c1(
            master_test_suite().argv[1],
            "--wait", "2",
            g,
            ec
    );

    bp::child c2(
            master_test_suite().argv[1],
            "--wait", "2",
            g,
            ec
    );

    BOOST_CHECK(c1.running());
    BOOST_CHECK(c2.running());

    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(c1.in_group(ec));
    BOOST_CHECK_MESSAGE(!ec, ec.message());
    BOOST_REQUIRE(c2.in_group(ec));
    BOOST_CHECK_MESSAGE(!ec, ec.message());
    g.wait(ec);

    BOOST_CHECK(!c1.running());
    BOOST_CHECK(!c2.running());
}


BOOST_AUTO_TEST_CASE(wait_group_test_timeout, *boost::unit_test::timeout(15))
{
    using boost::unit_test::framework::master_test_suite;

    std::atomic<bool> done{false};
    std::thread thr{
            [&]
            {
                for (int i = 0; i < 150 && !done.load(); i++)
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                BOOST_REQUIRE(done.load());
            }};

    std::error_code ec;
    bp::group g;


    bp::child c1(
            master_test_suite().argv[1],
            "--wait", "2",
            g,
            ec
    );

    bp::child c2(
            master_test_suite().argv[1],
            "--wait", "5",
            g,
            ec
    );

    BOOST_CHECK(c1.running());
    BOOST_CHECK(c2.running());

    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(c1.in_group());
    BOOST_REQUIRE(c2.in_group());

    BOOST_CHECK(!g.wait_for(std::chrono::milliseconds(2500), ec));

    BOOST_CHECK_MESSAGE(!ec, std::to_string(ec.value()) + " == " + ec.message());
    BOOST_CHECK(!c1.running());
    BOOST_CHECK(c2.running());

    BOOST_CHECK(g.wait_for(std::chrono::seconds(5), ec));
    BOOST_CHECK_MESSAGE(!ec, std::to_string(ec.value()) + " == " + ec.message());
    BOOST_CHECK(!c1.running());
    BOOST_CHECK(!c2.running());

    done.store(true);
    thr.join();
}



