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

namespace bp = boost::process;


namespace std
{
std::ostream & operator<<(std::ostream & str, const std::wstring & ws)
{
    str << bp::detail::convert(ws);
    return str;
}
}

BOOST_AUTO_TEST_CASE(empty)
{
    bp::environment ev ;
    BOOST_CHECK(ev.empty());
    BOOST_CHECK_EQUAL(ev.size(), 0u);
    BOOST_CHECK_EQUAL(ev.end() - ev.begin(), 0);
    ev["Thingy"] = "My value";

    BOOST_CHECK(!ev.empty());
    BOOST_CHECK_EQUAL(ev.size(), 1u);
    BOOST_CHECK_EQUAL(ev.end() - ev.begin(), 1);

    for (auto  x : ev)
    {
        BOOST_CHECK_EQUAL(x.to_string(), "My value");
        BOOST_CHECK_EQUAL(x.get_name(), "Thingy");
    }

    ev["Thingy"].clear();
    BOOST_CHECK(ev.empty());
    BOOST_CHECK_EQUAL(ev.size(), 0u);
    BOOST_CHECK_EQUAL(ev.end() - ev.begin(), 0);
    ev.clear();
}

BOOST_AUTO_TEST_CASE(wempty)
{
    bp::wenvironment ev ;
    BOOST_CHECK(ev.empty());
    BOOST_CHECK_EQUAL(ev.size(), 0u);
    BOOST_CHECK_EQUAL(ev.end() - ev.begin(), 0);
    ev[L"Thingy"] = L"My value";

    BOOST_CHECK(!ev.empty());
    BOOST_CHECK_EQUAL(ev.size(), 1u);
    BOOST_CHECK_EQUAL(ev.end() - ev.begin(), 1);

    for (auto  x : ev)
    {
        BOOST_CHECK(x.to_string() == L"My value");
        BOOST_CHECK(x.get_name()  == L"Thingy");
    }

    ev[L"Thingy"].clear();
    BOOST_CHECK(ev.empty());
    BOOST_CHECK_EQUAL(ev.size(), 0u);
    BOOST_CHECK_EQUAL(ev.end() - ev.begin(), 0);
    ev.clear();
}

BOOST_AUTO_TEST_CASE(compare)
{
    auto nat = boost::this_process::environment();
    bp::environment env = nat;

    {
        BOOST_CHECK_EQUAL(nat.size(), env.size());
        auto ni = nat.begin();
        auto ei = env.begin();

        while ((ni != nat.end()) &&(ei != env.end()))
        {
            BOOST_CHECK_EQUAL(ni->get_name(),  ei->get_name());
            BOOST_CHECK_EQUAL(ni->to_string(), ei->to_string());
            ni++; ei++;
        }
    }

    //ok check if I can convert it.
    bp::wenvironment wenv{env};
    auto wnat = boost::this_process::wenvironment();
    BOOST_CHECK_EQUAL(wenv.size(), env.size());
    BOOST_CHECK_EQUAL(wnat.size(), nat.size());
    {
        BOOST_CHECK_EQUAL(wnat.size(), wenv.size());
        auto ni = wnat.begin();
        auto ei = wenv.begin();

        while ((ni != wnat.end()) && (ei != wenv.end()))
        {
            BOOST_CHECK_EQUAL(ni->get_name() , ei->get_name());
            BOOST_CHECK_EQUAL(ni->to_string(), ei->to_string());
            ni++; ei++;
        }

        BOOST_CHECK(ni == wnat.end());
    }
    env.clear();
    wenv.clear();
}

BOOST_AUTO_TEST_CASE(insert_remove)
{
    bp::environment env(boost::this_process::environment());

    auto sz = env.size();
    BOOST_REQUIRE_GE(sz, 1u);
    BOOST_REQUIRE_EQUAL(env.count("BOOST_TEST_VAR"), 0u);

    env["BOOST_TEST_VAR"] = {"some string", "badabumm"};

#if defined(BOOST_WINDOWS_API)
    BOOST_CHECK_EQUAL(env["BOOST_TEST_VAR"].to_string(), "some string;badabumm");
#else
    BOOST_CHECK_EQUAL(env["BOOST_TEST_VAR"].to_string(), "some string:badabumm");
#endif
    BOOST_CHECK_EQUAL(sz +1, env.size());

    env["BOOST_TEST_VAR"].clear();

    BOOST_CHECK_EQUAL(env.size(), sz);

    env.clear();

}

BOOST_AUTO_TEST_CASE(andrejnau)
{
    bp::environment env;
    env["a"] = "1";
    env["b"] = "2";
    env["c"] = "3";
    env.clear();
}

