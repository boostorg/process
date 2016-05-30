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

#include <string>

namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(show_window)
{
    using boost::unit_test::framework::master_test_suite;

    bp::ipstream is;

	std::error_code ec;
	bp::child c(
		master_test_suite().argv[1],
		"test", "--windows-print-showwindow",
		bp::show_normal,
		bp::std_out>is,
		ec
	);
	BOOST_REQUIRE(!ec);

    int i;
    is >> i;
    BOOST_CHECK_EQUAL(i, SW_SHOWNORMAL);
}
