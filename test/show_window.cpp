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
#include <boost/process.hpp>
#include <boost/process/windows.hpp>
#include <boost/system/error_code.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>

namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(show_window)
{
    using boost::unit_test::framework::master_test_suite;
    namespace bio = boost::iostreams;

    bp::pipe p;
    bp::child c;
    {

        std::error_code ec;
        c = bp::execute(
            master_test_suite().argv[1],
            "test", "--windows-print-showwindow",
            bp::show_normal,
            bp::std_out>p,
            ec
        );
        BOOST_REQUIRE(!ec);
    }


    bio::stream<bio::file_descriptor_source> is(p.source());

    int i;
    is >> i;
    BOOST_CHECK_EQUAL(i, SW_SHOWNORMAL);
}
