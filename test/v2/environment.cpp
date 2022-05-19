// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// Test that header file is self-contained.
#include <boost/process/v2/environment.hpp>

#include <boost/test/unit_test.hpp>

namespace bp2 = boost::process::v2;
namespace bpe = boost::process::v2::environment;


BOOST_AUTO_TEST_CASE(environment)
{
    BOOST_CHECK_EQUAL(bp2::filesystem::current_path(),  bpe::get("PWD"));

    for (const auto & elem : bpe::get("PATH"))
        BOOST_CHECK(std::find(elem.begin(), elem.end(), bpe::delimiter) == elem.end());

    BOOST_CHECK(bpe::get("PATH").size() > 0);

    const auto key1 = "BP2_TEST_NAME";
#if defined(BOOST_PROCESS_V2_WINDOWS)
    const auto key2 = "BP2_TeSt_NamE";
#else
    const auto key2 = key1;
#endif

    BOOST_CHECK_THROW(bpe::get(key1) , bp2::system_error);
    bp2::error_code ec;
    bpe::get(key2, ec);
    BOOST_CHECK_EQUAL(ec, boost::system::errc::permission_denied);

    bpe::set(key1, "some test string");
    BOOST_CHECK(bpe::get(key1) == "some test string");
    BOOST_CHECK(bpe::get(key2) == "some test string");
    bpe::unset(key2);

    BOOST_CHECK_THROW(bpe::set("invalid=", "blablubb") , bp2::system_error);
    BOOST_CHECK_THROW(bpe::get(key1) , bp2::system_error);
    bpe::get(key2, ec);
    BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

    for (const auto ke : bpe::view())
        BOOST_CHECK_EQUAL(bpe::get(ke.get<0>()), ke.get<1>());

#if defined(BOOST_PROCESS_V2_POSIX)
    BOOST_CHECK_EQUAL(bpe::key("FOO"), bpe::key_view("FOO"));
    BOOST_CHECK_EQUAL(bpe::key("FOO"), std::string("FOO"));
    BOOST_CHECK_EQUAL(bpe::key_value_pair("FOO=BAR"), bpe::key_value_pair_view("FOO=BAR"));
    BOOST_CHECK_EQUAL(bpe::key_value_pair("FOO", "BAR"), bpe::key_value_pair_view("FOO=BAR"));

    using sv = bpe::value::string_type;
    std::string cmp = sv("FOO=X") + bpe::delimiter + sv("YY") + bpe::delimiter + sv("Z42");
    BOOST_CHECK_EQUAL(bpe::key_value_pair("FOO", {"X", "YY", "Z42"}), cmp);
#endif

}



BOOST_AUTO_TEST_CASE(wenvironment)
{
    BOOST_CHECK_EQUAL(bp2::filesystem::current_path(),  bpe::get(L"PWD"));

    for (const auto & elem : bpe::get(L"PATH"))
        BOOST_CHECK(std::find(elem.begin(), elem.end(), bpe::delimiter) == elem.end());

    BOOST_CHECK(bpe::get(L"PATH").size() > 0);

    const auto key1 = L"BP2_TEST_NAME";
#if defined(BOOST_PROCESS_V2_WINDOWS)
    const auto key2 = L"BP2_TeSt_NamE";
#else
    const auto key2 = key1;
#endif

    BOOST_CHECK_THROW(bpe::get(key1) , bp2::system_error);
    bp2::error_code ec;
    bpe::get(key2, ec);
    BOOST_CHECK_EQUAL(ec, boost::system::errc::permission_denied);

    bpe::set(key1, L"some test string");
    BOOST_CHECK(bpe::get(key1) == L"some test string");
    BOOST_CHECK(bpe::get(key2) == L"some test string");
    bpe::unset(key2);

    BOOST_CHECK_THROW(bpe::set(L"invalid=", L"blablubb") , bp2::system_error);
    BOOST_CHECK_THROW(bpe::get(key1) , bp2::system_error);
    bpe::get(key2, ec);
    BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

    for (const auto ke : bpe::view())
        BOOST_CHECK_EQUAL(bpe::get(ke.get<0>()), ke.get<1>());

#if defined(BOOST_PROCESS_V2_WINDOWS)
    BOOST_CHECK_EQUAL(bpe::key(L"FOO"), bpe::key_view(L"FOO"));
    BOOST_CHECK_EQUAL(bpe::key(L"FOO"), std::wstring(L"FOO"));
    BOOST_CHECK_EQUAL(bpe::key_value_pair(L"FOO=BAR"), bpe::key_value_pair_view(L"FOO=BAR"));
    BOOST_CHECK_EQUAL(bpe::key_value_pair(L"FOO", L"BAR"), bpe::key_value_pair_view(L"FOO=BAR"));

    using sv = bpe::value::string_type;
    std::string cmp = sv(L"FOO=X") + bpe::delimiter + sv(L"YY") + bpe::delimiter + sv(L"Z42");
    BOOST_CHECK_EQUAL(bpe::key_value_pair(L"FOO", {L"X", L"YY", L"Z42"}), cmp);

#endif
}

