// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#define BOOST_TEST_MAIN

#include <boost/test/included/unit_test.hpp>
#include <iostream>


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

    bp::opstream os(pipe);
    bp::ipstream is(pipe);

    int i = 42, j = 0;

    os << i <<  " String" << std::endl;
    os << std::endl;
    cout << "wrote" << endl;
    is >> j;
    cout << "J: '" << j << "'" << endl;


    BOOST_CHECK_EQUAL(i, j);
}



