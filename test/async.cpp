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

#include <boost/process/error.hpp>
#include <boost/process/exe_args.hpp>
#include <boost/process/async.hpp>
#include <boost/process/execute.hpp>

#include <boost/system/error_code.hpp>

namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(async_wait)
{
    using boost::unit_test::framework::master_test_suite;
    using namespace boost::asio;

    boost::asio::io_service io_service;

    int exit_code = 0;
    std::error_code ec;
    bp::child c = bp::execute(
        master_test_suite().argv[1],
        "test", "--exit-code", "123",
        ec,
        io_service,
        bp::on_exit([&](int exit, const std::error_code& ec){exit_code = exit; BOOST_REQUIRE(!ec);})
    );
    BOOST_REQUIRE(!ec);

    io_service.run();

    BOOST_CHECK_EQUAL(exit_code, 123);
}
