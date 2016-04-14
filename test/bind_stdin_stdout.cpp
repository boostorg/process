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

#include <boost/process/basic_cmd.hpp>
#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
#include <boost/process/child.hpp>
#include <boost/process/execute.hpp>
#include <system_error>

#include <boost/system/error_code.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>
#include <iostream>

namespace bp = boost::process;
namespace bio = boost::iostreams;

BOOST_AUTO_TEST_CASE(sync_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p1;
    bp::pipe p2;

    std::error_code ec;
    bp::execute(
        master_test_suite().argv[1],
        bp::args+={"test", "--stdin-to-stdout"},
        bp::std_in<p1,
        bp::std_out>p2,
        ec
    );
    BOOST_REQUIRE(!ec);

    bio::stream<bio::file_descriptor_sink> os(p1.sink());

    bio::stream<bio::file_descriptor_source> is(p2.source());

    std::string s = "abcdefghi j";
    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
    {
        os << *it << std::flush;
        char c;
        is >> std::noskipws >> c;
        BOOST_CHECK_EQUAL(*it, c);
    }
}
