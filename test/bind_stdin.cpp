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
#include <boost/process/io.hpp>
#include <boost/process/child.hpp>
#include <boost/process/args.hpp>
#include <boost/process/async.hpp>

#include <system_error>
#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>
#include <string>
#include <cstdlib>
#include <iostream>
#include <thread>


#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
typedef boost::asio::windows::stream_handle pipe_end;
#elif defined(BOOST_POSIX_API)
#   include <sys/wait.h>
#   include <unistd.h>
typedef boost::asio::posix::stream_descriptor pipe_end;
#endif



namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(sync_io, *boost::unit_test::timeout(2))
{
    std::cout << "sync_io" << std::endl;
    using boost::unit_test::framework::master_test_suite;

    bp::opstream os;
    bp::ipstream is;

    std::error_code ec;
  
    bp::child c(
        master_test_suite().argv[1],
        bp::args+={"test", "--prefix", "abc"},
        bp::std_in <os,
        bp::std_out>is,
        ec);
    
    BOOST_REQUIRE(!ec);
  
  

    os << "hello" << std::endl;
      
    std::string s;


    is >> s;
    BOOST_CHECK_EQUAL(s, "abchello");
    os << 123 << std::endl;
    is >> s;
    BOOST_CHECK_EQUAL(s, "abc123");
    os << 3.1415 << std::endl;
    is >> s;
    BOOST_CHECK_EQUAL(s, "abc3.1415");

}


struct write_handler
{
    bp::ipstream &is_;

    write_handler(bp::ipstream &is) : is_(is) {}

    void operator()(const boost::system::error_code &ec, std::size_t size)
    {
        BOOST_REQUIRE(!ec);
        BOOST_REQUIRE_EQUAL(6u, size);
        std::string s;
        is_ >> s;
        BOOST_CHECK_EQUAL(s, "abchello");
    }
};

BOOST_AUTO_TEST_CASE(async_io, *boost::unit_test::timeout(2))
{
    std::cout << "async_io" << std::endl;
    using boost::unit_test::framework::master_test_suite;

    boost::asio::io_service io_service;

    bp::async_pipe p1(io_service);
    bp::ipstream is;

    boost::asio::streambuf sb;
    std::ostream os(&sb);

    std::error_code ec;
    bp::child c(
        master_test_suite().argv[1],
        "test", "--prefix-once", "abc",
        bp::std_in<p1,
        bp::std_out>is,
        ec
    );
    BOOST_REQUIRE(!ec);

    os << "hello" << std::endl;

  //  std::string s = "hello\n";
    boost::asio::async_write(p1, sb,
        write_handler(is));

    io_service.run();

    c.wait();
}

BOOST_AUTO_TEST_CASE(nul, *boost::unit_test::timeout(2))
{
    std::cout << "nul" << std::endl;

    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;
    bp::child c(
        master_test_suite().argv[1],
        "test", "--is-nul-stdin",
        bp::std_in<bp::null,
        ec);

    BOOST_REQUIRE(!ec);

    c.wait();
    int exit_code = c.exit_code();
#if defined(BOOST_WINDOWS_API)
    BOOST_CHECK_EQUAL(EXIT_SUCCESS, exit_code);
#elif defined(BOOST_POSIX_API)
    BOOST_CHECK_EQUAL(EXIT_SUCCESS, WEXITSTATUS(exit_code));
#endif
}
