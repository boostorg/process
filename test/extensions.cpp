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
#include <boost/process/execute.hpp>
#include <boost/process/error.hpp>
#include <system_error>
#include <boost/bind.hpp>
#include <boost/ref.hpp>



namespace bp = boost::process;


struct run_exe
{
    std::string exe;
    template<typename T>
    void operator()(T &e) const
    {
        e.exe = exe.c_str();
    }

};

struct  set_on_error
{
    mutable std::error_code ec;
    template<typename T>
    void operator()(T &, const std::error_code & ec) const
    {
        this->ec = ec;
    }
};

BOOST_AUTO_TEST_CASE(extensions)
{
    using boost::unit_test::framework::master_test_suite;

    run_exe re;

    re.exe = master_test_suite().argv[1];

    set_on_error se;
    std::error_code ec;
    bp::execute(
#if defined(BOOST_WINDOWS_API)
        "Wrong-Command",
        "test",
        bp::on_setup=re,
        bp::on_error=se,
        bp::ignore_error
#endif
    );
    BOOST_CHECK(!se.ec);
}
