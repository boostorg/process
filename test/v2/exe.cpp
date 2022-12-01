// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/exe.hpp>

#include <boost/test/unit_test.hpp>

#include <vector>

BOOST_AUTO_TEST_CASE(test_exe)
{
    namespace bp2 = boost::process::v2;

    auto all = bp2::all_pids();
    std::vector<filesystem::path> pths;
    for (unsigned i = 0; i < all.size(); i++) {
        auto pth = bp2::exe_path(all[i]);
        pths.push_back((!pth.string().empty()) ? pth.string() : "<unknown>");
    }
    // Check failed if all executable paths are unknown.
    BOOST_CHECK(std::count(pths.begin(), pths.end(), "<unknown>") != pths.size());
}
