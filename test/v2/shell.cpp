// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// Test that header file is self-contained.
#include <boost/process/v2/shell.hpp>

#include <boost/test/unit_test.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
    #define STR(Value) L##Value
    #define STR_VIEW(Value) boost::process::v2::wcstring_ref(STR(Value))
#else
    #define STR(Value) Value
    #define STR_VIEW(Value) boost::process::v2::cstring_ref(STR(Value))
#endif


BOOST_AUTO_TEST_CASE(test_shell_parser)
{
    using boost::process::v2::shell;

    auto sh = shell(STR("foo \""));

    boost::system::error_code ec;
    auto argv = sh.parse(ec);
    BOOST_CHECK(argv.empty());
    BOOST_CHECK(ec);

    sh = shell(STR("foo bar \"foo bar\""));
    
    ec.clear();
    argv = sh.parse(ec);
    BOOST_CHECK(argv.argc() == 3u);
    BOOST_CHECK(!ec);
    BOOST_CHECK(argv.argv()[0] == STR_VIEW("foo"));
    BOOST_CHECK(argv.argv()[1] == STR_VIEW("bar"));
    BOOST_CHECK(argv.argv()[2] == STR_VIEW("foo bar"));
}