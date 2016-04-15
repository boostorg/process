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

#include <boost/process/environment.hpp>
#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
#include <boost/process/env.hpp>
#include <boost/process/execute.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <boost/system/error_code.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/program_options/environment_iterator.hpp>
#include <string>
#include <stdlib.h>

namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(inherit_env)
{
    using boost::unit_test::framework::master_test_suite;
    namespace bio = boost::iostreams;


    bp::pipe p;

    std::error_code ec;
    bp::execute(
        master_test_suite().argv[1],
        "test", "--query ", "PATH",
        bp::std_out>p,
        ec
    );
    BOOST_REQUIRE(!ec);

    bio::stream<bio::file_descriptor_source> st(p.source());
    std::string s;

    std::getline(st, s);

    auto path = bp::this_process::environment()["PATH"].to_string();

    BOOST_REQUIRE(!path.empty());
    auto size = (path.size() < s.size()) ? path.size() : s.size();

    BOOST_CHECK_EQUAL_COLLECTIONS(
            s.begin(),    s.   begin() + size,
            path.begin(), path.begin() + size
            );
}


BOOST_AUTO_TEST_CASE(inherit_mod_env)
{
    using boost::unit_test::framework::master_test_suite;
    namespace bio = boost::iostreams;

    auto ie = bp::this_process::environment();
    std::string value = "TestString";
    ie["BOOST_PROCESS_TEST_1"] = value;

    bp::pipe p;

    std::error_code ec;
    bp::execute(
        master_test_suite().argv[1],
        "test", "--query", "BOOST_PROCESS_TEST_1",
        bp::std_out>p,
        ec
    );
    BOOST_REQUIRE(!ec);

    bio::stream<bio::file_descriptor_source> st(p.source());
    std::string s;

    std::getline(st, s);

    auto size = (value.size() < s.size()) ? value.size() : s.size();

    BOOST_CHECK_EQUAL_COLLECTIONS(
            s.begin(),     s.    begin() + size,
            value.begin(), value.begin() + size
            );
}


BOOST_AUTO_TEST_CASE(modifided_env)
{
    using boost::unit_test::framework::master_test_suite;
    namespace bio = boost::iostreams;

    bp::pipe p;
    boost::process::environment env = boost::this_process::environment(); //empty env, that would fail.

    std::string value = "TestString";
    env["BOOST_PROCESS_TEST_2"] = value;


    std::error_code ec;
    bp::execute(
        master_test_suite().argv[1],
        "test", "--query", "BOOST_PROCESS_TEST_2",
        bp::std_out>p,
        env,
        ec
    );
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(boost::this_process::environment().count(value) == 0);

    bio::stream<bio::file_descriptor_source> st(p.source());
    std::string s;
    std::getline(st, s);

    BOOST_CHECK(boost::algorithm::starts_with(s, "TestString"));
}

BOOST_AUTO_TEST_CASE(this_env)
{
    /*auto env = boost::this_process::environment();

    for (auto e : env)
    {
        cout << e.get_name() << " : " << e.to_string() << endl;
    }

    BOOST_CHECK(false);*/


}
