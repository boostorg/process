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

#include <boost/process/basic_cmd.hpp>
#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
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

BOOST_AUTO_TEST_CASE(sync_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p;

    std::error_code ec;
    bp::execute(
        master_test_suite().argv[1],
        bp::args+={"test", "--echo-stdout", "hello"},
        bp::std_out>p,
        ec
    );
    BOOST_REQUIRE(!ec);

    bio::stream<bio::file_descriptor_source> is(p.source());

    std::string s;
    is >> s;
    BOOST_CHECK_EQUAL(s, "hello");
}


struct read_handler
{
    boost::asio::streambuf &buffer_;

    read_handler(boost::asio::streambuf &buffer) : buffer_(buffer) {}

    void operator()(const boost::system::error_code &ec, std::size_t size)
    {
        BOOST_REQUIRE(!ec);
        std::istream is(&buffer_);
        std::string line;
        std::getline(is, line);
        BOOST_CHECK(boost::algorithm::starts_with(line, "abc"));
    }
};

BOOST_AUTO_TEST_CASE(async_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p = bp::pipe::create_async();

    std::error_code ec;
    bp::execute(
        master_test_suite().argv[1],
        "test", "--echo-stdout", "abc",
        bp::std_out > p,
        ec
    );
    BOOST_REQUIRE(!ec);

    boost::asio::io_service io_service;
    pipe_end pend(io_service, p.source().handle());

    boost::asio::streambuf buffer;
    boost::asio::async_read_until(pend, buffer, '\n',
        read_handler(buffer));

    io_service.run();
}

BOOST_AUTO_TEST_CASE(nul)
{
    using boost::unit_test::framework::master_test_suite;


    std::error_code ec;
    bp::child c = bp::execute(
        master_test_suite().argv[1],
        bp::args+={"test", "--is-nul-stdout"},
        bp::std_out>bp::null,
        ec
    );
    BOOST_REQUIRE(!ec);

    c.wait();
    int exit_code = c.exit_code();
#if defined(BOOST_WINDOWS_API)
    BOOST_CHECK_EQUAL(EXIT_SUCCESS, exit_code);
#elif defined(BOOST_POSIX_API)
    BOOST_CHECK_EQUAL(EXIT_SUCCESS, WEXITSTATUS(exit_code));
#endif
}
