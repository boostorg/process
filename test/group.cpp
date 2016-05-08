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

#include <boost/system/error_code.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
#include <boost/process/args.hpp>
#include <boost/process/child.hpp>
#include <boost/process/group.hpp>
#include <boost/process/execute.hpp>
#include <system_error>

#include <string>
#include <istream>
#include <iostream>
#include <cstdlib>
#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
typedef boost::asio::windows::stream_handle pipe_end;
#elif defined(BOOST_POSIX_API)
#   include <sys/wait.h>
#   include <unistd.h>
typedef boost::asio::posix::stream_descriptor pipe_end;
#endif

namespace bp = boost::process;
namespace bio = boost::iostreams;

BOOST_AUTO_TEST_CASE(group_test)
{
    cout << "group_test" << endl;
    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;
    bp::group g;
    auto c = bp::execute(
        master_test_suite().argv[1],
        g,
        ec
    );
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(c.in_group());
    BOOST_CHECK(c);
    BOOST_CHECK(c.running());

    g.terminate();

    BOOST_CHECK(!c.running());
    if (c.running())
        c.terminate();
}
#define L() cout << __FILE__ "(" << __LINE__ << ")" << endl;
BOOST_AUTO_TEST_CASE(attached, *boost::unit_test::timeout(2))
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p;

    bp::group g;

    std::error_code ec;
    auto c = bp::execute(
        master_test_suite().argv[1],
        bp::args+={"--launch-attached"},
        bp::std_out>p,
        g,
        ec
    );
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(c.in_group());
    BOOST_CHECK(c);

    bio::stream<bio::file_descriptor_source> is(p.source());


    bp::pid_t pid;
    is >> pid;
    bp::child sub_c(pid);
    is >> pid; //invalid pid.


    BOOST_REQUIRE(sub_c);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); //just to be sure.
    g.terminate();
    BOOST_CHECK(sub_c);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); //just to be sure.

    BOOST_CHECK(!c.running());
    auto still_runs = sub_c.running();
    BOOST_CHECK(!still_runs);
    if (still_runs)
        sub_c.terminate();
    BOOST_CHECK(!c.running());
    if (c.running())
        c.terminate();

}



BOOST_AUTO_TEST_CASE(detached, *boost::unit_test::timeout(2))
{
    std::cerr << "detached" << std::endl;

    using boost::unit_test::framework::master_test_suite;

    bp::pipe p;

    bp::group g;


    std::error_code ec;
    auto c = bp::execute(
        master_test_suite().argv[1],
        bp::args+={"--launch-detached"},
        bp::std_out>p,
        g,
        ec
    );

    BOOST_REQUIRE(!ec);
    BOOST_CHECK(c);
    bio::stream<bio::file_descriptor_source> is(p.source());

    bp::pid_t pid;
    is >> pid;
    is >> pid;
    bp::child sub_c(pid);

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); //just to be sure.
    BOOST_CHECK(sub_c.running());

    g.terminate();
    BOOST_CHECK(sub_c);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); //just to be sure.

    auto still_runs = sub_c.running();
    BOOST_CHECK(still_runs);
    if (still_runs)
        sub_c.terminate();

    BOOST_CHECK(!c.running());
    if (c.running())
        c.terminate();
}


