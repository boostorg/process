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
#include <boost/process/execute.hpp>
#include <system_error>

#include <string>
#include <istream>
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

BOOST_AUTO_TEST_CASE(attached)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p;

    std::error_code ec;
    auto c = bp::execute(
        master_test_suite().argv[1],
        bp::args+={"--launch-attached"},
        bp::std_out>p,
        ec
    );
    BOOST_REQUIRE(!ec);

    bio::stream<bio::file_descriptor_source> is(p.source());

#if defined( BOOST_WINDOWS_API )
    std::intptr_t handle_p;
    is >> handle_p;
    auto handle = reinterpret_cast<void*>(handle_p);
    bp::child sub_c(handle);
#elif defined( BOOST_POSIX_API )
    pid_t pid;
    is >> pid;
    bp::child sub_c(pid);
#endif
    c.terminate();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //just to be sure.

    auto still_runs = sub_c.running();
    BOOST_CHECK(!still_runs);
    if (still_runs)
        sub_c.terminate();

}



BOOST_AUTO_TEST_CASE(detached)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p;

    std::error_code ec;
    auto c = bp::execute(
        master_test_suite().argv[1],
        bp::args+={"--launch-attached"},
        bp::std_out>p,
        ec
    );
    BOOST_REQUIRE(!ec);

    bio::stream<bio::file_descriptor_source> is(p.source());

#if defined( BOOST_WINDOWS_API )
    std::intptr_t handle_p;
    is >> handle_p;
    auto handle = reinterpret_cast<void*>(handle_p);
    bp::child sub_c(handle);
#elif defined( BOOST_POSIX_API )
    pid_t pid;
    is >> pid;
    bp::child sub_c(pid);
#endif

    c.terminate();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //just to be sure.

    BOOST_CHECK(sub_c.running());

    sub_c.terminate();
}



BOOST_AUTO_TEST_CASE(attached_detached)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p;

    std::error_code ec;
    auto c = bp::execute(
        master_test_suite().argv[1],
        bp::args+={"--launch-attached", "--launch-detached"},
        bp::std_out>p,
        ec
    );
    BOOST_REQUIRE(!ec);

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //just to be sure.

    bio::stream<bio::file_descriptor_source> is(p.source());
#if defined( BOOST_WINDOWS_API )
    std::intptr_t handle_p;
    is >> handle_p;
    auto handle = reinterpret_cast<void*>(handle_p);
    bp::child sub_c_a(handle);
    is >> handle_p;
    handle = reinterpret_cast<void*>(handle_p);
    bp::child sub_c_d(handle);

#elif defined( BOOST_POSIX_API )
    pid_t pid;
    is >> pid;
    bp::child sub_c_a(pid);
    is >> pid;
    bp::child sub_c_d(pid);
#endif


    c.terminate();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //just to be sure.

    auto still_runs = sub_c_a.running();
    BOOST_CHECK(!still_runs);
    if (still_runs)
        sub_c_a.terminate();


    BOOST_CHECK(sub_c_d.running());

    sub_c_d.terminate();



}
