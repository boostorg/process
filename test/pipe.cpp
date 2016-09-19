// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#define BOOST_TEST_MAIN

#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <thread>

#include <boost/process/pipe.hpp>

using namespace std;
namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(plain, *boost::unit_test::timeout(2))
{
    bp::pipe pipe;

    std::string in  = "test";
    pipe.write(in.c_str(), in.size());

    std::string out;
    out.resize(4);
    pipe.read(&out.front(), out.size());

    BOOST_CHECK_EQUAL(out, in);
}

BOOST_AUTO_TEST_CASE(named, *boost::unit_test::timeout(2))
{

#if defined( BOOST_WINDOWS_API )
    bp::pipe pipe("\\\\.\\pipe\\pipe_name");
#elif defined( BOOST_POSIX_API )
    bp::pipe pipe("./test_pipe");
#endif

    std::string in  = "xyz";
    pipe.write(in.c_str(), in.size());


    std::string out;
    out.resize(3);
    pipe.read(&out.front(), out.size());


    BOOST_CHECK_EQUAL(out, in);
}

BOOST_AUTO_TEST_CASE(copy_pipe, *boost::unit_test::timeout(2))
{
    bp::pipe pipe;

    std::string in  = "test";
    pipe.write(in.c_str(), in.size());

    std::string out;
    out.resize(4);
    auto p2 = pipe;
    p2.read(&out.front(), out.size());

    BOOST_CHECK_EQUAL(out, in);
}

BOOST_AUTO_TEST_CASE(move_pipe, *boost::unit_test::timeout(2))
{
    bp::pipe pipe;

    std::string in  = "test";
    pipe.write(in.c_str(), in.size());

    std::string out;
    out.resize(4);
    auto p2 = std::move(pipe);
    p2.read(&out.front(), out.size());

    BOOST_CHECK_EQUAL(out, in);
}

BOOST_AUTO_TEST_CASE(stream, *boost::unit_test::timeout(2))
{

    bp::pipe pipe;

    bp::pstream os(pipe);
    bp::ipstream is(pipe);

    int i = 42, j = 0;

    os << i << std::endl;
    os << std::endl;
    is >> j;

    BOOST_CHECK_EQUAL(i, j);
}

BOOST_AUTO_TEST_CASE(stream_line, *boost::unit_test::timeout(2))
{

    bp::pstream os;

    std::string s = "My Test String";

    std::string out;

    os << s <<  std::endl;

    std::getline(os, out);

    auto size = (out.size() < s.size()) ? out.size() : s.size();


    BOOST_CHECK_EQUAL_COLLECTIONS(
               s.begin(),   s.  begin() + size,
               out.begin(), out.begin() + size
               );
}


BOOST_AUTO_TEST_CASE(large_data, *boost::unit_test::timeout(2))
{
    bp::pipe pipe;

    bp::ipstream is(pipe);
    bp::opstream os(pipe);

    std::string in(1000000, '0');
    std::string out;

    int cnt = 0;
    for (auto & c: in)
        c = (cnt++ % 26) + 'A';

    std::thread th([&]{os << in << std::endl;});

    is >> out;
    BOOST_REQUIRE_EQUAL_COLLECTIONS(out.begin(), out.end(), in.begin(), in.end());
    th.join();
}


