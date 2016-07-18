// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MAIN
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>

#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
#include <boost/process/async.hpp>
#include <boost/process/child.hpp>
#include <boost/process/system.hpp>

#include <string>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>

#include <vector>
#include <array>

namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(cr_test, *boost::unit_test::timeout(5))
{
    //ok, here I check if I got the coroutine stuff right
    using boost::unit_test::framework::master_test_suite;

    std::vector<int> vec;

    boost::asio::io_service ios;
    auto stackful =
            [&](boost::asio::yield_context yield_)
            {
                vec.push_back(0);
                ios.post(yield_);
                vec.push_back(2);
                ios.post(yield_);
                vec.push_back(4);
            };

    struct stackless_t : boost::asio::coroutine
    {
        std::vector<int> &vec;
        boost::asio::io_service & ios;
        stackless_t(std::vector<int> & vec_,boost::asio::io_service & ios_) : vec(vec_), ios(ios_) {}
        void operator()(
                boost::system::error_code ec = boost::system::error_code(),
                std::size_t n = 0)
        {
            if (!ec) reenter (this)
            {
                vec.push_back(1);
                yield ios.post(*this);
                vec.push_back(3);
                yield ios.post(*this);
                vec.push_back(5);
            }
        }
    } stackless{vec, ios};

    boost::asio::spawn(ios, stackful);
    ios.post(stackless);

    ios.run();

    std::array<int, 6> cmp = {0,1,2,3,4,5};

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec.begin(), vec.end(),
            cmp.begin(), cmp.end());
}

BOOST_AUTO_TEST_CASE(stackful, *boost::unit_test::timeout(5))
{
    using boost::unit_test::framework::master_test_suite;

    bool did_something_else = false;

    boost::asio::io_service ios;
    auto stackful =
            [&](boost::asio::yield_context yield_)
            {
                std::error_code ec;
                auto ret =
                    bp::system(
                        master_test_suite().argv[1],
                        "test", "--exit-code", "123",
                        ec,
                        yield_);

                BOOST_CHECK(!ec);
                BOOST_CHECK_EQUAL(ret, 123);
                BOOST_CHECK(did_something_else);
            };

    boost::asio::spawn(ios, stackful);
    ios.post([&]{did_something_else = true;});

    ios.run();
    BOOST_CHECK(did_something_else);
}

BOOST_AUTO_TEST_CASE(stackful_error, *boost::unit_test::timeout(5))
{
    using boost::unit_test::framework::master_test_suite;

    bool did_something_else = false;

    boost::asio::io_service ios;
    auto stackful =
            [&](boost::asio::yield_context yield_)
            {
                std::error_code ec;
                auto ret =
                    bp::system("none-existing-exe",
                        ec,
                        yield_);

                BOOST_CHECK(ec);
                BOOST_CHECK_EQUAL(ret, -1);
                BOOST_CHECK(!did_something_else);
            };

    boost::asio::spawn(ios, stackful);
    ios.post([&]{did_something_else = true;});

    ios.run();

    BOOST_CHECK(did_something_else);
}


BOOST_AUTO_TEST_CASE(stackless, *boost::unit_test::timeout(5))
{
    using boost::unit_test::framework::master_test_suite;

    boost::asio::io_service ios;

    bool did_something_else = false;

    struct stackless_t : boost::asio::coroutine
    {
        boost::asio::io_service & ios;
        bool & did_something_else;

        int exit_code = -1;
        stackless_t(boost::asio::io_service & ios_,
                    bool & did_something_else)
                        : ios(ios_), did_something_else(did_something_else) {}
        void operator()(
                boost::system::error_code ec = boost::system::error_code(),
                std::size_t n = 0)
        {
            if (!ec) reenter (this)
            {
            	yield bp::child(master_test_suite().argv[1],
                        "test", "--exit-code", "42",
                        ios,
                        bp::on_exit=
                        [this](int ret, const std::error_code&)
                        {
            				exit_code = ret;
                            (*this)();
                        }).detach();

                BOOST_CHECK_EQUAL(exit_code, 42);
                BOOST_CHECK(did_something_else);
            }
        }
    } stackless{ios, did_something_else};

    ios.post([&]{stackless();});
    ios.post([&]{did_something_else = true;});

    ios.run();

    BOOST_CHECK(did_something_else);
}


